/* Copyright (c) 2012-2014 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE.md>
 */

#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "iterator.h"
#include "iterator_async.h"

namespace leveldown {

static v8::Persistent<v8::FunctionTemplate> iterator_constructor;

Iterator::Iterator (
    Database* database
  , uint32_t id
  , leveldb::Slice* start
  , std::string end
  , bool skipStart
  , bool skipEnd
  , bool noReverse
  , bool keys
  , bool values
  , int limit
  , bool fillCache
  , bool keyAsBuffer
  , bool valueAsBuffer
  , v8::Local<v8::Object> &startHandle
  , size_t highWaterMark
) : database(database)
  , id(id)
  , start(start)
  , end(end)
  , skipStart(skipStart)
  , skipEnd(skipEnd)
  , noReverse(noReverse)
  , limit(limit)
  , highWaterMark(highWaterMark)
  , keys(keys)
  , values(values)
  , keyAsBuffer(keyAsBuffer)
  , valueAsBuffer(valueAsBuffer)
{
  NanScope();

  v8::Local<v8::Object> obj = NanNew<v8::Object>();
  if (!startHandle.IsEmpty())
    obj->Set(NanNew("start"), startHandle);
  NanAssignPersistent(persistentHandle, obj);

  options    = leveldb::ReadOptions();
  options.fill_cache = fillCache;
  dbIterator = NULL;
  count      = 0;
  nexting    = false;
  ended      = false;
  endWorker  = NULL;
  InitDbIterator();
};

Iterator::~Iterator () {
  IteratorEnd();

  if (!persistentHandle.IsEmpty())
    NanDisposePersistent(persistentHandle);

  if (start != NULL) {
    delete[] start->data();
    delete start;
  }
};

inline void Iterator::InitDbIterator() {
  // get a snapshot of the current state
  options.snapshot = database->NewSnapshot();
  dbIterator = database->NewIterator(&options);
  /*
  printf("skipStart=%d, skipEnd=%d\n", skipStart, skipEnd);
  printf("noReverse=%d\n", noReverse);
  if (end) printf("end=%s\n", end->c_str());
  */
  if (start) {
    //printf("start=%s\n", start->ToString().c_str());
    dbIterator->Seek(*start);
    if (noReverse) {
      if (dbIterator->Valid()) {
        std::string key_ = dbIterator->key().ToString();
        if (skipStart && key_ == start->ToString())
          dbIterator->Next();
      }
    } else {
      if (!dbIterator->Valid()) {
        // if it's past the last key, step back
        dbIterator->SeekToLast();
      } else {
        std::string key_ = dbIterator->key().ToString();
        if (key_ > start->ToString()) dbIterator->Prev();
        if (skipStart && key_ == start->ToString()) dbIterator->Prev();
      }
    }
  }
  else {
    if (noReverse)
      dbIterator->SeekToFirst();
    else
      dbIterator->SeekToLast();
  }
}

bool Iterator::GetIterator () {
  if (dbIterator == NULL) {
    InitDbIterator();
    return true;
  }
  return false;
}

//read and move to next
inline bool Iterator::Read (std::string& key, std::string& value) {
  bool result = dbIterator->Valid() && (limit < 0 || ++count <= limit);
  // now check if this is the end or not, if not then return the key & value
  if (result) {
    //key = dbIterator->key().ToString();
    //key.assign(dbIterator->key().data(), dbIterator->key().size());
    key = dbIterator->key().ToString();
    result = end.empty() || (noReverse ? (skipEnd ? key < end : key <= end) : (skipEnd ? key > end : key >= end));

    if (result) {
      if (values)
        value = dbIterator->value().ToString();
        //value.assign(dbIterator->value().data(), dbIterator->value().size());
    }
  }
  if (result) {
    if (noReverse)
      dbIterator->Next();
    else
      dbIterator->Prev();
  }

  return result;
}

//another implementation for performance.
bool Iterator::IteratorNext2 (std::vector<std::pair<std::string, std::string> >& results) {
  size_t size = 0;
  bool result = (limit < 0 || ++count <= limit);

  if (result) {
    if (noReverse) {
      for (;result && dbIterator->Valid();dbIterator->Next()) {
        std::string key, value;
        key = dbIterator->key().ToString();
        result = (end.empty() || key <= end);
        if (result) {
          if (skipEnd && !end.empty() && key == end) {
            result = false;
            break;
          }
          value = dbIterator->value().ToString();
          results.push_back(std::make_pair(key, value));
          //(*results)[key] = value;
          size = size + key.size() + value.size();
          if (size >= highWaterMark) {
            dbIterator->Next();
            break;
          }
        }
        result = result && (limit < 0 || ++count <= limit);
      }
    }
    else {
      for (;result && dbIterator->Valid();dbIterator->Prev()) {
        std::string key, value;
        key = dbIterator->key().ToString();
        result = (end.empty() || key >= end);
        if (result) {
          if (skipEnd && !end.empty() && key == end) {
            result = false;
            break;
          }
          value = dbIterator->value().ToString();
          results.push_back(std::make_pair(key, value));
          //(*results)[key] = value;
          size = size + key.size() + value.size();
          if (size >= highWaterMark) {
            dbIterator->Prev();
            break;
          }
        }
        result = result && (limit < 0 || ++count <= limit);
      }
    }
  }
  return result && dbIterator->Valid();
}

bool Iterator::IteratorNext (std::vector<std::pair<std::string, std::string> >& results) {
  size_t size = 0;
  bool ok;
  do {
    std::string key, value;
    ok = Read(key, value);

    if (ok) {
      results.push_back(std::make_pair(key, value));
      size = size + key.size() + value.size();

      if (size > highWaterMark)
        return true;

    } else {
      return false;
    }
  } while(ok);
  return ok;
/*
  size_t size = 0;
  std::string key, value;
  bool result = dbIterator->Valid() && (limit < 0 || ++count <= limit);

  if (result) do {
    key = dbIterator->key().ToString();
    if (noReverse)
      result = (end.empty() || key <= end);
    else
      result = (end.empty() || key >= end);
    if (result) {
      //if (end)
      //  printf("%s <= %s  = %d\n", key.c_str(), end->c_str(), key <= *end);
      if (skipEnd && !end.empty() && key == end) {
        result = false;
        break;
      }
      //if (!keys) key.clear();
      //if (values)
      value = dbIterator->value().ToString();
        //value.assign(dbIterator->value().data(), dbIterator->value().size());
      results.push_back(std::make_pair(key, value));
      //(*results)[key] = value;
      size = size + key.size() + value.size();
      if (size >= highWaterMark) {
        if (noReverse) dbIterator->Next();
        else dbIterator->Prev();
        break;
      }
    }
    if (noReverse) dbIterator->Next();
    else dbIterator->Prev();
    result = result && dbIterator->Valid() && (limit < 0 || ++count <= limit);
  } while (result);
  return result;
  //*/
}

leveldb::Status Iterator::IteratorStatus () {
  return dbIterator->status();
}

void Iterator::IteratorEnd () {
  if (dbIterator) {
    //TODO: could return it->status()
    delete dbIterator;
    dbIterator = NULL;
    database->ReleaseSnapshot(options.snapshot);
  }
}

void Iterator::Release () {
  database->ReleaseIterator(id);
}

void checkEndCallback (Iterator* iterator) {
  iterator->nexting = false;
  if (iterator->endWorker != NULL) {
    NanAsyncQueueWorker(iterator->endWorker);
    iterator->endWorker = NULL;
  }
}

//nextSync()
//return the array(2), 
//  the first is the result array, 
//  the second is the count of the result. if count <=0 means no more data.
NAN_METHOD(Iterator::NextSync) {
  NanScope();

  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());
  iterator->nexting = true;
  std::vector<std::pair<std::string, std::string> > dict;
  bool ok = iterator->IteratorNext2(dict);
  checkEndCallback(iterator);
  if (!ok) {
    leveldb::Status s = iterator->IteratorStatus();
    if (!s.ok()) {
      NanThrowError(s.ToString().c_str());
      NanReturnUndefined();
    }
  }

  size_t idx = 0;
  size_t arraySize = dict.size()*2;
  v8::Local<v8::Array> returnArray = NanNew<v8::Array>(arraySize);//args[0].As<v8::Array>();
  for(idx = 0; idx < dict.size(); ++idx) {
    std::pair<std::string, std::string> row = dict[idx];
    std::string key = row.first;
    std::string value = row.second;

    v8::Local<v8::Value> returnKey;
    if (iterator->keys) {
      if (iterator->keyAsBuffer) {
        returnKey = NanNewBufferHandle((char*)key.data(), key.size());
      } else {
        returnKey = NanNew<v8::String>((char*)key.data(), key.size());
      }
    }
    else
      returnKey = NanNull();
    v8::Local<v8::Value> returnValue;
    if (iterator->values) {
      if (iterator->valueAsBuffer) {
        returnValue = NanNewBufferHandle((char*)value.data(), value.size());
      } else {
        returnValue = NanNew<v8::String>((char*)value.data(), value.size());
      }
    }
    else
      returnValue = NanNull();

    
    //returnArray->Set(NanNew<v8::Integer>(static_cast<int>(idx * 2)), returnKey);
    //returnArray->Set(NanNew<v8::Integer>(static_cast<int>(idx * 2+1)), returnValue);
    //returnArray->Set(returnKey, returnValue);
    // put the key & value in a descending order, so that they can be .pop:ed in javascript-land
    returnArray->Set(NanNew<v8::Integer>(static_cast<int>(arraySize - idx * 2 - 1)), returnKey);
    returnArray->Set(NanNew<v8::Integer>(static_cast<int>(arraySize - idx * 2 - 2)), returnValue);
  }

  
  ssize_t s =  dict.size();
  if (!ok) s = -s;
  v8::Local<v8::Array> result = NanNew<v8::Array>(2);
  result->Set(NanNew<v8::Integer>(0), returnArray);
  // when ok === false all data has been read, so it's then finished
  result->Set(NanNew<v8::Integer>(1), NanNew<v8::Integer>(s));

  NanReturnValue(result);
}

NAN_METHOD(Iterator::EndSync) {
  NanScope();

  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());
  if (iterator->nexting) {
    NanReturnValue(NanFalse());
  }
  if (!iterator->ended) {
    iterator->ended = true;
    iterator->IteratorEnd();
    iterator->Release();
    NanReturnValue(NanTrue());
  }
  NanReturnUndefined();
}

NAN_METHOD(Iterator::Next) {
  NanScope();

  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());

  v8::Local<v8::Function> callback = args[0].As<v8::Function>();

  NextWorker* worker = new NextWorker(
      iterator
    , new NanCallback(callback)
    , checkEndCallback
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("iterator", _this);
  iterator->nexting = true;
  NanAsyncQueueWorker(worker);

  NanReturnValue(args.Holder());
}

NAN_METHOD(Iterator::End) {
  NanScope();

  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());

  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);

  EndWorker* worker = new EndWorker(
      iterator
    , new NanCallback(callback)
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = args.This();
  worker->SaveToPersistent("iterator", _this);
  iterator->ended = true;

  if (iterator->nexting) {
    // waiting for a next() to return, queue the end
    iterator->endWorker = worker;
  } else {
    NanAsyncQueueWorker(worker);
  }

  NanReturnValue(args.Holder());
}

void Iterator::Init () {
  v8::Local<v8::FunctionTemplate> tpl =
      NanNew<v8::FunctionTemplate>(Iterator::New);
  NanAssignPersistent(iterator_constructor, tpl);
  tpl->SetClassName(NanNew("Iterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Iterator::Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "nextSync", Iterator::NextSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "end", Iterator::End);
  NODE_SET_PROTOTYPE_METHOD(tpl, "endSync", Iterator::EndSync);
}

v8::Local<v8::Object> Iterator::NewInstance (
        v8::Local<v8::Object> database
      , v8::Local<v8::Number> id
      , v8::Local<v8::Object> optionsObj
    ) {

  NanEscapableScope();

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanNew<v8::FunctionTemplate>(iterator_constructor);

  if (optionsObj.IsEmpty()) {
    v8::Handle<v8::Value> argv[2] = { database, id };
    instance = constructorHandle->GetFunction()->NewInstance(2, argv);
  } else {
    v8::Handle<v8::Value> argv[3] = { database, id, optionsObj };
    instance = constructorHandle->GetFunction()->NewInstance(3, argv);
  }

  return NanEscapeScope(instance);
}

NAN_METHOD(Iterator::New) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());

  v8::Local<v8::Object> startHandle;
  leveldb::Slice* start = NULL;
  std::string end;
  bool skipStart = false;
  bool skipEnd = false;
  int limit = -1;
  // default highWaterMark from Readble-streams
  size_t highWaterMark = 16 * 1024;

  v8::Local<v8::Value> id = args[1];

  v8::Local<v8::Object> optionsObj;

  v8::Local<v8::Object> ltHandle;
  v8::Local<v8::Object> lteHandle;
  v8::Local<v8::Object> gtHandle;
  v8::Local<v8::Object> gteHandle;

  //default to forward.
  bool reverse = false;

  if (args.Length() > 1 && args[2]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(args[2]);

    reverse = NanBooleanOptionValue(optionsObj, NanNew("reverse"));

    if (optionsObj->Has(NanNew("start"))
        && (node::Buffer::HasInstance(optionsObj->Get(NanNew("start")))
          || optionsObj->Get(NanNew("start"))->IsString())) {

      startHandle = optionsObj->Get(NanNew("start")).As<v8::Object>();

      // ignore start if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(startHandle) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_start, startHandle, start)
        start = new leveldb::Slice(_start.data(), _start.size());
      }
    }

    if (optionsObj->Has(NanNew("end"))
        && (node::Buffer::HasInstance(optionsObj->Get(NanNew("end")))
          || optionsObj->Get(NanNew("end"))->IsString())) {

      v8::Local<v8::Value> endBuffer = optionsObj->Get(NanNew("end"));

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(endBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_end, endBuffer, end)
        end = std::string(_end.data(), _end.size());
        delete[] _end.data();
      }
    }

    if (!optionsObj.IsEmpty() && optionsObj->Has(NanNew("limit"))) {
      limit = v8::Local<v8::Integer>::Cast(optionsObj->Get(
          NanNew("limit")))->Value();
    }

    if (optionsObj->Has(NanNew("highWaterMark"))) {
      highWaterMark = v8::Local<v8::Integer>::Cast(optionsObj->Get(
            NanNew("highWaterMark")))->Value();
    }

    if (optionsObj->Has(NanNew("lt"))
        && (node::Buffer::HasInstance(optionsObj->Get(NanNew("lt")))
          || optionsObj->Get(NanNew("lt"))->IsString())) {

      v8::Local<v8::Value> ltBuffer = optionsObj->Get(NanNew("lt"));

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(ltBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_lt, ltBuffer, lt)
        if (reverse) {
          if (!start || _lt.ToString() <= start->ToString()) {
            if (start) {
              delete[] start->data();
              delete start;
            }
            start = new leveldb::Slice(_lt.data(), _lt.size());
            skipStart = true;
          } else
            delete[] _lt.data();
        }
        else {
          if (end.empty() || _lt.ToString() <= end) {
            end = std::string(_lt.data(), _lt.size());
            skipEnd = true;
          }
          delete[] _lt.data();
        }
      }
    }

    if (optionsObj->Has(NanNew("lte"))
        && (node::Buffer::HasInstance(optionsObj->Get(NanNew("lte")))
          || optionsObj->Get(NanNew("lte"))->IsString())) {

      v8::Local<v8::Value> lteBuffer = optionsObj->Get(NanNew("lte"));

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(lteBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_lte, lteBuffer, lte)
        //lte = new std::string(_lte.data(), _lte.size());
        if (reverse) {
          if (!start || _lte.ToString() < start->ToString()) {
            if (start) {
              delete[] start->data();
              delete start;
            }
            start = new leveldb::Slice(_lte.data(), _lte.size());
            skipStart = false;
          } else
            delete[] _lte.data();
        }
        else {
          if (end.empty() || _lte.ToString() < end) {
            end = std::string(_lte.data(), _lte.size());
            skipEnd = false;
          }
          delete[] _lte.data();
        }
      }
    }

    if (optionsObj->Has(NanNew("gt"))
        && (node::Buffer::HasInstance(optionsObj->Get(NanNew("gt")))
          || optionsObj->Get(NanNew("gt"))->IsString())) {

      v8::Local<v8::Value> gtBuffer = optionsObj->Get(NanNew("gt"));

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(gtBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_gt, gtBuffer, gt)
        //gt = new std::string(_gt.data(), _gt.size());
        if (!reverse) {
          if (!start || _gt.ToString() >= start->ToString()) {
            if (start) {
              delete[] start->data();
              delete start;
            }
            start = new leveldb::Slice(_gt.data(), _gt.size());
            skipStart = true;
          } else
            delete[] _gt.data();
        } else {
          if (end.empty() || _gt.ToString() >= end) {
            end = std::string(_gt.data(), _gt.size());
            skipEnd = true;
          }
          delete[] _gt.data();
        }
      }
    }

    if (optionsObj->Has(NanNew("gte"))
        && (node::Buffer::HasInstance(optionsObj->Get(NanNew("gte")))
          || optionsObj->Get(NanNew("gte"))->IsString())) {

      v8::Local<v8::Value> gteBuffer = optionsObj->Get(NanNew("gte"));

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(gteBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_gte, gteBuffer, gte)
        //gte = new std::string(_gte.data(), _gte.size());
        if (!reverse) {
          if (!start || _gte.ToString() > start->ToString()) {
            if (start) {
              delete[] start->data();
              delete start;
            }
            start = new leveldb::Slice(_gte.data(), _gte.size());
            skipStart = false;
          } else
            delete[] _gte.data();
        } else {
          if (end.empty() || _gte.ToString() > end) {
            end = std::string(_gte.data(), _gte.size());
            skipEnd = false;
          }
          delete[] _gte.data();
        }
      }
    }

  }

  bool keys = NanBooleanOptionValue(optionsObj, NanNew("keys"), true);
  bool values = NanBooleanOptionValue(optionsObj, NanNew("values"), true);
  bool keyAsBuffer = NanBooleanOptionValue(
      optionsObj
    , NanNew("keyAsBuffer")
    , true
  );
  bool valueAsBuffer = NanBooleanOptionValue(
      optionsObj
    , NanNew("valueAsBuffer")
    , true
  );
  bool fillCache = NanBooleanOptionValue(optionsObj, NanNew("fillCache"));

  Iterator* iterator = new Iterator(
      database
    , (uint32_t)id->Int32Value()
    , start
    , end
    , skipStart
    , skipEnd
    , !reverse
    , keys
    , values
    , limit
    , fillCache
    , keyAsBuffer
    , valueAsBuffer
    , startHandle
    , highWaterMark
  );
  iterator->Wrap(args.This());

  NanReturnValue(args.This());
}

} // namespace leveldown

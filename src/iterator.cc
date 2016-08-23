/* Copyright (c) 2012-2016 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "iterator.h"
#include "iterator_async.h"
#include "common.h"

namespace leveldown {

static Nan::Persistent<v8::FunctionTemplate> iterator_constructor;

Iterator::Iterator (
    Database* database
  , uint32_t id
  , leveldb::Slice* start
  , std::string* end
  , bool reverse
  , bool keys
  , bool values
  , int limit
  , std::string* lt
  , std::string* lte
  , std::string* gt
  , std::string* gte
  , bool fillCache
  , bool keyAsBuffer
  , bool valueAsBuffer
  , size_t highWaterMark
) : database(database)
  , id(id)
  , start(start)
  , end(end)
  , reverse(reverse)
  , keys(keys)
  , values(values)
  , limit(limit)
  , lt(lt)
  , lte(lte)
  , gt(gt)
  , gte(gte)
  , highWaterMark(highWaterMark)
  , keyAsBuffer(keyAsBuffer)
  , valueAsBuffer(valueAsBuffer)
{
  Nan::HandleScope scope;

  options    = new leveldb::ReadOptions();
  options->fill_cache = fillCache;
  // get a snapshot of the current state
  options->snapshot = database->NewSnapshot();
  dbIterator = NULL;
  count      = 0;
  seeking    = false;
  nexting    = false;
  ended      = false;
  endWorker  = NULL;
};

Iterator::~Iterator () {
  delete options;
  if (start != NULL) {
    // Special case for `start` option: it won't be
    // freed up by any of the delete calls below.
    if (!((lt != NULL && reverse)
        || (lte != NULL && reverse)
        || (gt != NULL && !reverse)
        || (gte != NULL && !reverse))) {
      delete[] start->data();
    }
    delete start;
  }
  if (end != NULL)
    delete end;
  if (lt != NULL)
    delete lt;
  if (gt != NULL)
    delete gt;
  if (lte != NULL)
    delete lte;
  if (gte != NULL)
    delete gte;
};

bool Iterator::GetIterator () {
  if (dbIterator == NULL) {
    dbIterator = database->NewIterator(options);

    if (start != NULL) {
      dbIterator->Seek(*start);

      if (reverse) {
        if (!dbIterator->Valid()) {
          // if it's past the last key, step back
          dbIterator->SeekToLast();
        } else {
          std::string key_ = dbIterator->key().ToString();

          if (lt != NULL) {
            if (lt->compare(key_) <= 0)
              dbIterator->Prev();
          } else if (lte != NULL) {
            if (lte->compare(key_) < 0)
              dbIterator->Prev();
          } else if (start != NULL) {
            if (start->compare(key_))
              dbIterator->Prev();
          }
        }

        if (dbIterator->Valid() && lt != NULL) {
          if (lt->compare(dbIterator->key().ToString()) <= 0)
            dbIterator->Prev();
        }
      } else {
        if (dbIterator->Valid() && gt != NULL
            && gt->compare(dbIterator->key().ToString()) == 0)
          dbIterator->Next();
      }
    } else if (reverse) {
      dbIterator->SeekToLast();
    } else {
      dbIterator->SeekToFirst();
    }

    return true;
  }
  return false;
}

bool Iterator::Read (std::string& key, std::string& value) {
  // if it's not the first call, move to next item.
  if (!GetIterator() && !seeking) {
    if (reverse)
      dbIterator->Prev();
    else
      dbIterator->Next();
  }

  seeking = false;

  // now check if this is the end or not, if not then return the key & value
  if (dbIterator->Valid()) {
    std::string key_ = dbIterator->key().ToString();
    int isEnd = end == NULL ? 1 : end->compare(key_);

    if ((limit < 0 || ++count <= limit)
      && (end == NULL
          || (reverse && (isEnd <= 0))
          || (!reverse && (isEnd >= 0)))
      && ( lt  != NULL ? (lt->compare(key_) > 0)
         : lte != NULL ? (lte->compare(key_) >= 0)
         : true )
      && ( gt  != NULL ? (gt->compare(key_) < 0)
         : gte != NULL ? (gte->compare(key_) <= 0)
         : true )
    ) {
      if (keys)
        key.assign(dbIterator->key().data(), dbIterator->key().size());
      if (values)
        value.assign(dbIterator->value().data(), dbIterator->value().size());
      return true;
    }
  }

  return false;
}

bool Iterator::IteratorNext (std::vector<std::pair<std::string, std::string> >& result) {
  size_t size = 0;
  while(true) {
    std::string key, value;
    bool ok = Read(key, value);

    if (ok) {
      result.push_back(std::make_pair(key, value));
      size = size + key.size() + value.size();

      if (size > highWaterMark)
        return true;

    } else {
      return false;
    }
  }
}

leveldb::Status Iterator::IteratorStatus () {
  return dbIterator->status();
}

void Iterator::IteratorEnd () {
  //TODO: could return it->status()
  delete dbIterator;
  dbIterator = NULL;
  database->ReleaseSnapshot(options->snapshot);
}

void Iterator::Release () {
  database->ReleaseIterator(id);
}

void checkEndCallback (Iterator* iterator) {
  iterator->nexting = false;
  if (iterator->endWorker != NULL) {
    Nan::AsyncQueueWorker(iterator->endWorker);
    iterator->endWorker = NULL;
  }
}

NAN_METHOD(Iterator::Seek) {
  Iterator* iterator = Nan::ObjectWrap::Unwrap<Iterator>(info.This());
  iterator->GetIterator();
  leveldb::Iterator* dbIterator = iterator->dbIterator;
  Nan::Utf8String key(info[0]);

  dbIterator->Seek(*key);
  iterator->seeking = true;

  if (dbIterator->Valid()) {
    int cmp = dbIterator->key().compare(*key);
    if (cmp > 0 && iterator->reverse) {
      dbIterator->Prev();
    } else if (cmp < 0 && !iterator->reverse) {
      dbIterator->Next();
    }
  } else {
    if (iterator->reverse) {
      dbIterator->SeekToLast();
    } else {
      dbIterator->SeekToFirst();
    }
    if (dbIterator->Valid()) {
      int cmp = dbIterator->key().compare(*key);
      if (cmp > 0 && iterator->reverse) {
        dbIterator->SeekToFirst();
        dbIterator->Prev();
      } else if (cmp < 0 && !iterator->reverse) {
        dbIterator->SeekToLast();
        dbIterator->Next();
      }
    }
  }

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Iterator::Next) {
  Iterator* iterator = Nan::ObjectWrap::Unwrap<Iterator>(info.This());

  if (!info[0]->IsFunction()) {
    return Nan::ThrowError("next() requires a callback argument");
  }

  v8::Local<v8::Function> callback = info[0].As<v8::Function>();

  NextWorker* worker = new NextWorker(
      iterator
    , new Nan::Callback(callback)
    , checkEndCallback
  );
  // persist to prevent accidental GC
  v8::Local<v8::Object> _this = info.This();
  worker->SaveToPersistent("iterator", _this);
  iterator->nexting = true;
  Nan::AsyncQueueWorker(worker);

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Iterator::End) {
  Iterator* iterator = Nan::ObjectWrap::Unwrap<Iterator>(info.This());

  if (!info[0]->IsFunction()) {
    return Nan::ThrowError("end() requires a callback argument");
  }

  if (!iterator->ended) {
    v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[0]);

    EndWorker* worker = new EndWorker(
        iterator
      , new Nan::Callback(callback)
    );
    // persist to prevent accidental GC
    v8::Local<v8::Object> _this = info.This();
    worker->SaveToPersistent("iterator", _this);
    iterator->ended = true;

    if (iterator->nexting) {
      // waiting for a next() to return, queue the end
      iterator->endWorker = worker;
    } else {
      Nan::AsyncQueueWorker(worker);
    }
  }

  info.GetReturnValue().Set(info.Holder());
}

void Iterator::Init () {
  v8::Local<v8::FunctionTemplate> tpl =
      Nan::New<v8::FunctionTemplate>(Iterator::New);
  iterator_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New("Iterator").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "seek", Iterator::Seek);
  Nan::SetPrototypeMethod(tpl, "next", Iterator::Next);
  Nan::SetPrototypeMethod(tpl, "end", Iterator::End);
}

v8::Local<v8::Object> Iterator::NewInstance (
        v8::Local<v8::Object> database
      , v8::Local<v8::Number> id
      , v8::Local<v8::Object> optionsObj
    ) {

  Nan::EscapableHandleScope scope;

  Nan::MaybeLocal<v8::Object> maybeInstance;
  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> constructorHandle =
      Nan::New<v8::FunctionTemplate>(iterator_constructor);

  if (optionsObj.IsEmpty()) {
    v8::Local<v8::Value> argv[2] = { database, id };
    maybeInstance = Nan::NewInstance(constructorHandle->GetFunction(), 2, argv);
  } else {
    v8::Local<v8::Value> argv[3] = { database, id, optionsObj };
    maybeInstance = Nan::NewInstance(constructorHandle->GetFunction(), 3, argv);
  }

  if (maybeInstance.IsEmpty())
      Nan::ThrowError("Could not create new Iterator instance");
  else
    instance = maybeInstance.ToLocalChecked();
  return scope.Escape(instance);
}

NAN_METHOD(Iterator::New) {
  Database* database = Nan::ObjectWrap::Unwrap<Database>(info[0]->ToObject());

  leveldb::Slice* start = NULL;
  std::string* end = NULL;
  int limit = -1;
  // default highWaterMark from Readble-streams
  size_t highWaterMark = 16 * 1024;

  v8::Local<v8::Value> id = info[1];

  v8::Local<v8::Object> optionsObj;

  v8::Local<v8::Object> ltHandle;
  v8::Local<v8::Object> lteHandle;
  v8::Local<v8::Object> gtHandle;
  v8::Local<v8::Object> gteHandle;

  char *startStr = NULL;
  std::string* lt = NULL;
  std::string* lte = NULL;
  std::string* gt = NULL;
  std::string* gte = NULL;

  //default to forward.
  bool reverse = false;

  if (info.Length() > 1 && info[2]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(info[2]);

    reverse = BooleanOptionValue(optionsObj, "reverse");

    if (optionsObj->Has(Nan::New("start").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("start").ToLocalChecked()))
          || optionsObj->Get(Nan::New("start").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> startBuffer = optionsObj->Get(Nan::New("start").ToLocalChecked());

      // ignore start if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(startBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(_start, startBuffer, start)
        start = new leveldb::Slice(_startCh_, _startSz_);
        startStr = _startCh_;
      }
    }

    if (optionsObj->Has(Nan::New("end").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("end").ToLocalChecked()))
          || optionsObj->Get(Nan::New("end").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> endBuffer = optionsObj->Get(Nan::New("end").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(endBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(_end, endBuffer, end)
        end = new std::string(_endCh_, _endSz_);
        delete[] _endCh_;
      }
    }

    if (!optionsObj.IsEmpty() && optionsObj->Has(Nan::New("limit").ToLocalChecked())) {
      limit = v8::Local<v8::Integer>::Cast(optionsObj->Get(
          Nan::New("limit").ToLocalChecked()))->Value();
    }

    if (optionsObj->Has(Nan::New("highWaterMark").ToLocalChecked())) {
      highWaterMark = v8::Local<v8::Integer>::Cast(optionsObj->Get(
            Nan::New("highWaterMark").ToLocalChecked()))->Value();
    }

    if (optionsObj->Has(Nan::New("lt").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("lt").ToLocalChecked()))
          || optionsObj->Get(Nan::New("lt").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> ltBuffer = optionsObj->Get(Nan::New("lt").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(ltBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(_lt, ltBuffer, lt)
        lt = new std::string(_ltCh_, _ltSz_);
        delete[] _ltCh_;
        if (reverse) {
          if (startStr != NULL) {
            delete[] startStr;
            startStr = NULL;
          }
          if (start != NULL)
            delete start;
          start = new leveldb::Slice(lt->data(), lt->size());
        }
      }
    }

    if (optionsObj->Has(Nan::New("lte").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("lte").ToLocalChecked()))
          || optionsObj->Get(Nan::New("lte").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> lteBuffer = optionsObj->Get(Nan::New("lte").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(lteBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(_lte, lteBuffer, lte)
        lte = new std::string(_lteCh_, _lteSz_);
        delete[] _lteCh_;
        if (reverse) {
          if (startStr != NULL) {
            delete[] startStr;
            startStr = NULL;
          }
          if (start != NULL)
            delete start;
          start = new leveldb::Slice(lte->data(), lte->size());
        }
      }
    }

    if (optionsObj->Has(Nan::New("gt").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("gt").ToLocalChecked()))
          || optionsObj->Get(Nan::New("gt").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> gtBuffer = optionsObj->Get(Nan::New("gt").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(gtBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(_gt, gtBuffer, gt)
        gt = new std::string(_gtCh_, _gtSz_);
        delete[] _gtCh_;
        if (!reverse) {
          if (startStr != NULL) {
            delete[] startStr;
            startStr = NULL;
          }
          if (start != NULL)
            delete start;
          start = new leveldb::Slice(gt->data(), gt->size());
        }
      }
    }

    if (optionsObj->Has(Nan::New("gte").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("gte").ToLocalChecked()))
          || optionsObj->Get(Nan::New("gte").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> gteBuffer = optionsObj->Get(Nan::New("gte").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(gteBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(_gte, gteBuffer, gte)
        gte = new std::string(_gteCh_, _gteSz_);
        delete[] _gteCh_;
        if (!reverse) {
          if (startStr != NULL) {
            delete[] startStr;
            startStr = NULL;
          }
          if (start != NULL)
            delete start;
          start = new leveldb::Slice(gte->data(), gte->size());
        }
      }
    }

  }

  bool keys = BooleanOptionValue(optionsObj, "keys", true);
  bool values = BooleanOptionValue(optionsObj, "values", true);
  bool keyAsBuffer = BooleanOptionValue(optionsObj, "keyAsBuffer", true);
  bool valueAsBuffer = BooleanOptionValue(optionsObj, "valueAsBuffer", true);
  bool fillCache = BooleanOptionValue(optionsObj, "fillCache");

  Iterator* iterator = new Iterator(
      database
    , (uint32_t)id->Int32Value()
    , start
    , end
    , reverse
    , keys
    , values
    , limit
    , lt
    , lte
    , gt
    , gte
    , fillCache
    , keyAsBuffer
    , valueAsBuffer
    , highWaterMark
  );
  iterator->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

} // namespace leveldown

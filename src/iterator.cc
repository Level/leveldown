/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
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
  , v8::Local<v8::Object> &startHandle
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
  , keyAsBuffer(keyAsBuffer)
  , valueAsBuffer(valueAsBuffer)
{
  Nan::HandleScope scope;

  v8::Local<v8::Object> obj = Nan::New<v8::Object>();
  if (!startHandle.IsEmpty())
    obj->Set(Nan::New("start").ToLocalChecked(), startHandle);
  persistentHandle.Reset(obj);

  options    = new leveldb::ReadOptions();
  options->fill_cache = fillCache;
  dbIterator = NULL;
  count      = 0;
  nexting    = false;
  ended      = false;
  endWorker  = NULL;
};

Iterator::~Iterator () {
  delete options;
  if (!persistentHandle.IsEmpty())
    persistentHandle.Reset();
  if (start != NULL)
    delete start;
  if (end != NULL)
    delete end;
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

bool Iterator::IteratorNext (std::string& key, std::string& value) {
  // if it's not the first call, move to next item.
  if (!GetIterator()) {
    if (reverse)
      dbIterator->Prev();
    else
      dbIterator->Next();
  }

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

leveldb::Status Iterator::IteratorStatus () {
  return dbIterator->status();
}

void Iterator::IteratorEnd () {
  //TODO: could return it->status()
  delete dbIterator;
  dbIterator = NULL;
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

NAN_METHOD(Iterator::Next) {
  Iterator* iterator = Nan::ObjectWrap::Unwrap<Iterator>(info.This());

  if (info.Length() == 0 || !info[0]->IsFunction())
    return Nan::ThrowError("next() requires a callback argument");

  v8::Local<v8::Function> callback = info[0].As<v8::Function>();

  if (iterator->ended) {
    LD_RETURN_CALLBACK_OR_ERROR(callback, "cannot call next() after end()")
  }

  if (iterator->nexting) {
    LD_RETURN_CALLBACK_OR_ERROR(callback, "cannot call next() before previous next() has completed")
  }

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

  if (info.Length() == 0 || !info[0]->IsFunction())
    return Nan::ThrowError("end() requires a callback argument");

  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[0]);

  if (iterator->ended) {
    LD_RETURN_CALLBACK_OR_ERROR(callback, "end() already called on iterator")
  }

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

  info.GetReturnValue().Set(info.Holder());
}

void Iterator::Init () {
  v8::Local<v8::FunctionTemplate> tpl =
      Nan::New<v8::FunctionTemplate>(Iterator::New);
  iterator_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New("Iterator").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "next", Iterator::Next);
  Nan::SetPrototypeMethod(tpl, "end", Iterator::End);
}

v8::Local<v8::Object> Iterator::NewInstance (
        v8::Local<v8::Object> database
      , v8::Local<v8::Number> id
      , v8::Local<v8::Object> optionsObj
    ) {

  Nan::EscapableHandleScope scope;

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> constructorHandle =
      Nan::New<v8::FunctionTemplate>(iterator_constructor);

  if (optionsObj.IsEmpty()) {
    v8::Local<v8::Value> argv[2] = { database, id };
    instance = constructorHandle->GetFunction()->NewInstance(2, argv);
  } else {
    v8::Local<v8::Value> argv[3] = { database, id, optionsObj };
    instance = constructorHandle->GetFunction()->NewInstance(3, argv);
  }

  return scope.Escape(instance);
}

NAN_METHOD(Iterator::New) {
  Database* database = Nan::ObjectWrap::Unwrap<Database>(info[0]->ToObject());

  //TODO: remove this, it's only here to make LD_STRING_OR_BUFFER_TO_SLICE happy
  v8::Local<v8::Function> callback;

  v8::Local<v8::Object> startHandle;
  leveldb::Slice* start = NULL;
  std::string* end = NULL;
  int limit = -1;

  v8::Local<v8::Value> id = info[1];

  v8::Local<v8::Object> optionsObj;

  v8::Local<v8::Object> ltHandle;
  v8::Local<v8::Object> lteHandle;
  v8::Local<v8::Object> gtHandle;
  v8::Local<v8::Object> gteHandle;

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

      startHandle = optionsObj->Get(Nan::New("start").ToLocalChecked()).As<v8::Object>();

      // ignore start if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(startHandle) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_start, startHandle, start)
        start = new leveldb::Slice(_start.data(), _start.size());
      }
    }

    if (optionsObj->Has(Nan::New("end").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("end").ToLocalChecked()))
          || optionsObj->Get(Nan::New("end").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> endBuffer = optionsObj->Get(Nan::New("end").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(endBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_end, endBuffer, end)
        end = new std::string(_end.data(), _end.size());
      }
    }

    if (!optionsObj.IsEmpty() && optionsObj->Has(Nan::New("limit").ToLocalChecked())) {
      limit = v8::Local<v8::Integer>::Cast(optionsObj->Get(
        Nan::New("limit").ToLocalChecked()))->Value();
    }

    if (optionsObj->Has(Nan::New("lt").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("lt").ToLocalChecked()))
          || optionsObj->Get(Nan::New("lt").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> ltBuffer = optionsObj->Get(Nan::New("lt").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(ltBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_lt, ltBuffer, lt)
        lt = new std::string(_lt.data(), _lt.size());
        if (reverse)
          start = new leveldb::Slice(_lt.data(), _lt.size());
      }
    }

    if (optionsObj->Has(Nan::New("lte").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("lte").ToLocalChecked()))
          || optionsObj->Get(Nan::New("lte").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> lteBuffer = optionsObj->Get(Nan::New("lte").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(lteBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_lte, lteBuffer, lte)
        lte = new std::string(_lte.data(), _lte.size());
        if (reverse)
          start = new leveldb::Slice(_lte.data(), _lte.size());
      }
    }

    if (optionsObj->Has(Nan::New("gt").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("gt").ToLocalChecked()))
          || optionsObj->Get(Nan::New("gt").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> gtBuffer = optionsObj->Get(Nan::New("gt").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(gtBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_gt, gtBuffer, gt)
        gt = new std::string(_gt.data(), _gt.size());
        if (!reverse)
          start = new leveldb::Slice(_gt.data(), _gt.size());
      }
    }

    if (optionsObj->Has(Nan::New("gte").ToLocalChecked())
        && (node::Buffer::HasInstance(optionsObj->Get(Nan::New("gte").ToLocalChecked()))
          || optionsObj->Get(Nan::New("gte").ToLocalChecked())->IsString())) {

      v8::Local<v8::Value> gteBuffer = optionsObj->Get(Nan::New("gte").ToLocalChecked());

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(gteBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_gte, gteBuffer, gte)
        gte = new std::string(_gte.data(), _gte.size());
        if (!reverse)
          start = new leveldb::Slice(_gte.data(), _gte.size());
      }
    }

  }

  Iterator* iterator = new Iterator(
      database
    , (uint32_t)id->Int32Value()
    , start
    , end
    , reverse
    , BooleanOptionValue(optionsObj, "keys", true)
    , BooleanOptionValue(optionsObj, "values", true)
    , limit
    , lt
    , lte
    , gt
    , gte
    , BooleanOptionValue(optionsObj, "fillCache")
    , BooleanOptionValue(optionsObj, "keyAsBuffer", true)
    , BooleanOptionValue(optionsObj, "valueAsBuffer", true)
    , startHandle
  );
  iterator->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

} // namespace leveldown

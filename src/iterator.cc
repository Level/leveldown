/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
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
  , std::string* end
  , bool reverse
  , bool keys
  , bool values
  , int limit
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
  , keyAsBuffer(keyAsBuffer)
  , valueAsBuffer(valueAsBuffer)
{
  NanScope();

  v8::Local<v8::Object> obj = v8::Object::New();
  if (!startHandle.IsEmpty())
    obj->Set(NanSymbol("start"), startHandle);
  NanAssignPersistent(v8::Object, persistentHandle, obj);

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
    NanDispose(persistentHandle);
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
        if (!dbIterator->Valid())
          dbIterator->SeekToLast();
        else if (start->compare(dbIterator->key()))
          dbIterator->Prev();
      }
    }
    else if (reverse)
      dbIterator->SeekToLast();
    else
      dbIterator->SeekToFirst();
    return true;
  }
  return false;
}

bool Iterator::IteratorNext (std::string& key, std::string& value) {
  if (!GetIterator()) {
    if (reverse)
      dbIterator->Prev();
    else
      dbIterator->Next();
  }

  // 'end' here is an inclusive test
  if (dbIterator->Valid()
      && (limit < 0 || ++count <= limit)
      && (end == NULL
          || (reverse && end->compare(dbIterator->key().ToString()) <= 0)
          || (!reverse && end->compare(dbIterator->key().ToString()) >= 0))) {

    if (keys)
      key.assign(dbIterator->key().data(), dbIterator->key().size());
    if (values)
      value.assign(dbIterator->value().data(), dbIterator->value().size());
    return true;

  } else {
    return false;
  }
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
    NanAsyncQueueWorker(iterator->endWorker);
    iterator->endWorker = NULL;
  }
}

//void *ctx, void (*callback)(void *ctx, leveldb::Slice key, leveldb::Slice value)
NAN_METHOD(Iterator::Next) {
  NanScope();

  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());

  if (args.Length() == 0 || !args[0]->IsFunction())
    return NanThrowError("next() requires a callback argument");

  v8::Local<v8::Function> callback = args[0].As<v8::Function>();

  if (iterator->ended) {
    LD_RETURN_CALLBACK_OR_ERROR(callback, "cannot call next() after end()")
  }

  if (iterator->nexting) {
    LD_RETURN_CALLBACK_OR_ERROR(callback, "cannot call next() before previous next() has completed")
  }

  NextWorker* worker = new NextWorker(
      iterator
    , new NanCallback(callback)
    , checkEndCallback
  );
  iterator->nexting = true;
  NanAsyncQueueWorker(worker);

  NanReturnValue(args.Holder());
}

NAN_METHOD(Iterator::End) {
  NanScope();

  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());

  if (args.Length() == 0 || !args[0]->IsFunction())
    return NanThrowError("end() requires a callback argument");

  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);

  if (iterator->ended) {
    LD_RETURN_CALLBACK_OR_ERROR(callback, "end() already called on iterator")
  }

  EndWorker* worker = new EndWorker(
      iterator
    , new NanCallback(callback)
  );
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
      v8::FunctionTemplate::New(Iterator::New);
  NanAssignPersistent(v8::FunctionTemplate, iterator_constructor, tpl);
  tpl->SetClassName(NanSymbol("Iterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Iterator::Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "end", Iterator::End);
}

v8::Local<v8::Object> Iterator::NewInstance (
        v8::Local<v8::Object> database
      , v8::Local<v8::Number> id
      , v8::Local<v8::Object> optionsObj
    ) {

  NanScope();

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanPersistentToLocal(iterator_constructor);

  if (optionsObj.IsEmpty()) {
    v8::Handle<v8::Value> argv[2] = { database, id };
    instance = constructorHandle->GetFunction()->NewInstance(2, argv);
  } else {
    v8::Handle<v8::Value> argv[3] = { database, id, optionsObj };
    instance = constructorHandle->GetFunction()->NewInstance(3, argv);
  }

  return instance;
}

NAN_METHOD(Iterator::New) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());

  //TODO: remove this, it's only here to make LD_STRING_OR_BUFFER_TO_SLICE happy
  v8::Handle<v8::Function> callback;

  v8::Local<v8::Object> startHandle;
  leveldb::Slice* start = NULL;
  std::string* end = NULL;
  int limit = -1;

  v8::Local<v8::Value> id = args[1];

  v8::Local<v8::Object> optionsObj;

  if (args.Length() > 1 && args[2]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(args[2]);

    if (optionsObj->Has(NanSymbol("start"))
        && (node::Buffer::HasInstance(optionsObj->Get(NanSymbol("start")))
          || optionsObj->Get(NanSymbol("start"))->IsString())) {

      startHandle = optionsObj->Get(NanSymbol("start")).As<v8::Object>();

      // ignore start if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(startHandle) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_start, startHandle, start)
        start = new leveldb::Slice(_start.data(), _start.size());
      }
    }

    if (optionsObj->Has(NanSymbol("end"))
        && (node::Buffer::HasInstance(optionsObj->Get(NanSymbol("end")))
          || optionsObj->Get(NanSymbol("end"))->IsString())) {

      v8::Local<v8::Value> endBuffer =
          v8::Local<v8::Value>::New(optionsObj->Get(NanSymbol("end")));

      // ignore end if it has size 0 since a Slice can't have length 0
      if (StringOrBufferLength(endBuffer) > 0) {
        LD_STRING_OR_BUFFER_TO_SLICE(_end, endBuffer, end)
        end = new std::string(_end.data(), _end.size());
      }
    }

    if (!optionsObj.IsEmpty() && optionsObj->Has(NanSymbol("limit"))) {
      limit = v8::Local<v8::Integer>::Cast(optionsObj->Get(
          NanSymbol("limit")))->Value();
    }
  }

  bool reverse = NanBooleanOptionValue(optionsObj, NanSymbol("reverse"));
  bool keys = NanBooleanOptionValue(optionsObj, NanSymbol("keys"), true);
  bool values = NanBooleanOptionValue(optionsObj, NanSymbol("values"), true);
  bool keyAsBuffer = NanBooleanOptionValue(
      optionsObj
    , NanSymbol("keyAsBuffer")
    , true
  );
  bool valueAsBuffer = NanBooleanOptionValue(
      optionsObj
    , NanSymbol("valueAsBuffer")
    , true
  );
  bool fillCache = NanBooleanOptionValue(optionsObj, NanSymbol("fillCache"));

  Iterator* iterator = new Iterator(
      database
    , (uint32_t)id->Int32Value()
    , start
    , end
    , reverse
    , keys
    , values
    , limit
    , fillCache
    , keyAsBuffer
    , valueAsBuffer
    , startHandle
  );
  iterator->Wrap(args.This());

  NanReturnValue(args.This());
}

} // namespace leveldown

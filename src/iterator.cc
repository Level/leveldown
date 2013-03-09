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

Iterator::Iterator (
    Database* database
  , leveldb::Slice* start
  , std::string* end
  , bool reverse
  , bool keys
  , bool values
  , int limit
  , bool fillCache
  , bool keyAsBuffer
  , bool valueAsBuffer
  , v8::Persistent<v8::Value> startPtr
) : database(database)
  , start(start)
  , end(end)
  , reverse(reverse)
  , keys(keys)
  , values(values)
  , limit(limit)
  , keyAsBuffer(keyAsBuffer)
  , valueAsBuffer(valueAsBuffer)
  , startPtr(startPtr)
{
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
  startPtr.Dispose();
  if (start != NULL)
    delete start;
  if (end != NULL)
    delete end;
};

bool Iterator::GetIterator () {
  if (dbIterator == NULL) {
    dbIterator = database->NewIterator(options);
    if (start != NULL)
      dbIterator->Seek(*start);
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

void checkEndCallback (Iterator* iterator) {
  iterator->nexting = false;
  if (iterator->endWorker != NULL) {
    AsyncQueueWorker(iterator->endWorker);
    iterator->endWorker = NULL;
  }
}

//void *ctx, void (*callback)(void *ctx, leveldb::Slice key, leveldb::Slice value)
v8::Handle<v8::Value> Iterator::Next (const v8::Arguments& args) {
  v8::HandleScope scope;
  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());

  if (args.Length() == 0 || !args[0]->IsFunction()) {
    LD_THROW_RETURN("next() requires a callback argument")
  }

  if (iterator->ended) {
    LD_THROW_RETURN("Cannot call next() after end()")
  }

  if (iterator->nexting) {
    LD_THROW_RETURN("Cannot call next() before previous next() has completed")
  }

  v8::Persistent<v8::Function> callback =
      v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(args[0]));

  NextWorker* worker = new NextWorker(
      iterator
    , callback
    , checkEndCallback
  );
  iterator->nexting = true;
  AsyncQueueWorker(worker);

  return v8::Undefined();
}

v8::Handle<v8::Value> Iterator::End (const v8::Arguments& args) {
  v8::HandleScope scope;
  Iterator* iterator = node::ObjectWrap::Unwrap<Iterator>(args.This());

  if (args.Length() == 0 || !args[0]->IsFunction()) {
    LD_THROW_RETURN("end() requires a callback argument")
  }

  if (iterator->ended) {
    LD_THROW_RETURN("end() already called on iterator")
  }

  v8::Persistent<v8::Function> callback =
      v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(args[0]));

  EndWorker* worker = new EndWorker(
      iterator
    , callback
  );
  iterator->ended = true;

  if (iterator->nexting) {
    // waiting for a next() to return, queue the end
    iterator->endWorker = worker;
  } else {
    AsyncQueueWorker(worker);
  }

  return v8::Undefined();
}

v8::Persistent<v8::Function> Iterator::constructor;

void Iterator::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
  tpl->SetClassName(v8::String::NewSymbol("Iterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("next")
    , v8::FunctionTemplate::New(Next)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("end")
    , v8::FunctionTemplate::New(End)->GetFunction()
  );
  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
}

v8::Handle<v8::Value> Iterator::NewInstance (
        v8::Handle<v8::Object> database
      , v8::Handle<v8::Object> optionsObj
    ) {

  v8::HandleScope scope;
  v8::Local<v8::Object> instance;

  if (optionsObj.IsEmpty()) {
    v8::Handle<v8::Value> argv[1] = { database };
    instance = constructor->NewInstance(1, argv);
  } else {
    v8::Handle<v8::Value> argv[2] = { database, optionsObj };
    instance = constructor->NewInstance(2, argv);
  }

  return scope.Close(instance);
}

v8::Handle<v8::Value> Iterator::New (const v8::Arguments& args) {
  v8::HandleScope scope;

  //TODO: remove this, it's only here to make LD_STRING_OR_BUFFER_TO_SLICE happy
  v8::Local<v8::Function> callback;

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());

  v8::Local<v8::Value> startBuffer;
  leveldb::Slice* start = NULL;
  std::string* end = NULL;
  int limit = -1;

  v8::Local<v8::Object> optionsObj;

  if (args.Length() > 1 && args[1]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(args[1]);

    if (optionsObj->Has(option_start)
        && (node::Buffer::HasInstance(optionsObj->Get(option_start))
          || optionsObj->Get(option_start)->IsString())) {

      startBuffer = v8::Local<v8::Value>::New(optionsObj->Get(option_start));
      LD_STRING_OR_BUFFER_TO_SLICE(_start, startBuffer)
      start = new leveldb::Slice(_start.data(), _start.size());
    }

    if (optionsObj->Has(option_end)
        && (node::Buffer::HasInstance(optionsObj->Get(option_end))
          || optionsObj->Get(option_end)->IsString())) {

      v8::Local<v8::Value> endBuffer =
          v8::Local<v8::Value>::New(optionsObj->Get(option_end));
      LD_STRING_OR_BUFFER_TO_SLICE(_end, endBuffer)
      end = new std::string(_end.data(), _end.size());
    }

    if (!optionsObj.IsEmpty() && optionsObj->Has(option_limit)) {
      limit =
        v8::Local<v8::Integer>::Cast(optionsObj->Get(option_limit))->Value();
    }
  }

  LD_BOOLEAN_OPTION_VALUE(optionsObj, reverse)
  LD_BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, keys)
  LD_BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, values)
  LD_BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, keyAsBuffer)
  LD_BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, valueAsBuffer)
  LD_BOOLEAN_OPTION_VALUE(optionsObj, fillCache)

  Iterator* iterator = new Iterator(
      database
    , start
    , end
    , reverse
    , keys
    , values
    , limit
    , fillCache
    , keyAsBuffer
    , valueAsBuffer
    , v8::Persistent<v8::Value>::New(startBuffer)
  );
  iterator->Wrap(args.This());

  return args.This();
}

} // namespace leveldown

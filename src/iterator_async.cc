/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "leveldown.h"
#include "async.h"
#include "iterator_async.h"

namespace leveldown {

/** NEXT WORKER **/

NextWorker::NextWorker (
    Iterator* iterator
  , NanCallback *callback
  , void (*localCallback)(Iterator*)
) : AsyncWorker(NULL, callback)
  , iterator(iterator)
  , localCallback(localCallback)
{};

NextWorker::~NextWorker () {}

void NextWorker::Execute () {
  ok = iterator->IteratorNext(key, value);
  if (!ok)
    SetStatus(iterator->IteratorStatus());
}

void NextWorker::HandleOKCallback () {
  NanScope();

  v8::Local<v8::Value> returnKey;
  if (iterator->keyAsBuffer) {
    returnKey = NanNewBufferHandle((char*)key.data(), key.size());
  } else {
    returnKey = NanNew<v8::String>((char*)key.data(), key.size());
  }

  v8::Local<v8::Value> returnValue;
  if (iterator->valueAsBuffer) {
    returnValue = NanNewBufferHandle((char*)value.data(), value.size());
  } else {
    returnValue = NanNew<v8::String>((char*)value.data(), value.size());
  }

  // clean up & handle the next/end state see iterator.cc/checkEndCallback
  localCallback(iterator);

  if (ok) {
    v8::Local<v8::Value> argv[] = {
        NanNew<v8::Primitive>(NanNull())
      , returnKey
      , returnValue
    };
    callback->Call(3, argv);
  } else {
    callback->Call(0, NULL);
  }
}

/** NEXT-MULTI WORKER **/

NextBufferingWorker::NextBufferingWorker (
    Iterator* iterator
  , NanCallback *callback
  , void (*localCallback)(Iterator*)
) : AsyncWorker(NULL, callback)
  , iterator(iterator)
  , localCallback(localCallback)
{};

NextBufferingWorker::~NextBufferingWorker () {}

void NextBufferingWorker::Execute () {
  ok = iterator->IteratorNextBuffering(result);
  if (!ok)
    SetStatus(iterator->IteratorStatus());
}

void NextBufferingWorker::HandleOKCallback () {
  size_t idx = 0;
  v8::Local<v8::Array> returnArray;
  if (ok)
    returnArray = v8::Local<v8::Array>::New(v8::Array::New(result.size()));
  else
    // make room for a null-value at the end
    returnArray = v8::Local<v8::Array>::New(v8::Array::New(result.size() + 1));

  for(idx = 0; idx < result.size(); ++idx) {
    std::pair<std::string, std::string> row = result[idx];
    std::string key = row.first;
    std::string value = row.second;

    v8::Local<v8::Value> returnKey;
    if (iterator->keyAsBuffer) {
      returnKey = NanNewBufferHandle((char*)key.data(), key.size());
    } else {
      returnKey = NanNew<v8::String>((char*)key.data(), key.size());
    }

    v8::Local<v8::Value> returnValue;
    if (iterator->valueAsBuffer) {
      returnValue = NanNewBufferHandle((char*)value.data(), value.size());
    } else {
      returnValue = NanNew<v8::String>((char*)value.data(), value.size());
    }

    v8::Local<v8::Object> returnObject = v8::Local<v8::Object>::New(v8::Object::New());
    returnObject->Set(v8::String::NewSymbol("key"), returnKey);
    returnObject->Set(v8::String::NewSymbol("value"), returnValue);
    returnArray->Set(v8::Integer::New(static_cast<int>(idx)), returnObject);
  }

  // clean up & handle the next/end state see iterator.cc/checkEndCallback
  localCallback(iterator);

  if (!ok) {
    v8::Local<v8::Value> argv[] = {
        v8::Local<v8::Value>::New(v8::Null())
      , returnArray
    };
    callback->Call(2, argv);
  } else {
    callback->Call(0, NULL);
  }
}

/** END WORKER **/

EndWorker::EndWorker (
    Iterator* iterator
  , NanCallback *callback
) : AsyncWorker(NULL, callback)
  , iterator(iterator)
{};

EndWorker::~EndWorker () {}

void EndWorker::Execute () {
  iterator->IteratorEnd();
}

void EndWorker::HandleOKCallback () {
  iterator->Release();
  callback->Call(0, NULL);
}

} // namespace leveldown

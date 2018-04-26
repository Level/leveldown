/* Copyright (c) 2012-2018 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "leveldown.h"
#include "async.h"
#include "iterator_async.h"

namespace leveldown {

/** NEXT-MULTI WORKER **/

NextWorker::NextWorker(Iterator* iterator,
                       Nan::Callback *callback,
                       void (*localCallback)(Iterator*))
  : AsyncWorker(NULL, callback, "leveldown:iterator.next"), iterator(iterator),
    localCallback(localCallback)
{}

void NextWorker::Execute() {
  ok = iterator->IteratorNext(result);
  if (!ok)
    SetStatus(iterator->IteratorStatus());
}

void NextWorker::HandleOKCallback() {
  Nan::HandleScope scope;
  size_t idx = 0;

  size_t arraySize = result.size() * 2;
  v8::Local<v8::Array> returnArray = Nan::New<v8::Array>(arraySize);

  for(idx = 0; idx < result.size(); ++idx) {
    std::pair<std::string, std::string> row = result[idx];
    std::string key = row.first;
    std::string value = row.second;

    v8::Local<v8::Value> returnKey;
    if (iterator->keyAsBuffer) {
      //TODO: use NewBuffer, see database_async.cc
      returnKey = Nan::CopyBuffer((char*)key.data(), key.size()).ToLocalChecked();
    } else {
      returnKey = Nan::New<v8::String>((char*)key.data(), key.size()).ToLocalChecked();
    }

    v8::Local<v8::Value> returnValue;
    if (iterator->valueAsBuffer) {
      //TODO: use NewBuffer, see database_async.cc
      returnValue = Nan::CopyBuffer((char*)value.data(), value.size()).ToLocalChecked();
    } else {
      returnValue = Nan::New<v8::String>((char*)value.data(), value.size()).ToLocalChecked();
    }

    // put the key & value in a descending order, so that they can be .pop:ed in javascript-land
    returnArray->Set(Nan::New<v8::Integer>(static_cast<int>(arraySize - idx * 2 - 1)), returnKey);
    returnArray->Set(Nan::New<v8::Integer>(static_cast<int>(arraySize - idx * 2 - 2)), returnValue);
  }

  // clean up & handle the next/end state see iterator.cc/checkEndCallback
  localCallback(iterator);

  v8::Local<v8::Value> argv[] = {
      Nan::Null()
    , returnArray
    // when ok === false all data has been read, so it's then finished
    , Nan::New<v8::Boolean>(!ok)
  };
  callback->Call(3, argv, async_resource);
}

/** END WORKER **/

EndWorker::EndWorker(Iterator* iterator, Nan::Callback *callback)
  : AsyncWorker(NULL, callback, "leveldown:iterator.end"), iterator(iterator)
{}

void EndWorker::Execute() {
  iterator->IteratorEnd();
}

void EndWorker::HandleOKCallback() {
  iterator->Release();
  callback->Call(0, NULL, async_resource);
}

} // namespace leveldown

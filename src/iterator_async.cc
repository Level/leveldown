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
  , Nan::Callback *callback
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
  v8::Local<v8::Value> returnKey;
  if (iterator->keyAsBuffer) {
    returnKey = Nan::CopyBuffer((char*)key.data(), key.size()).ToLocalChecked();
  } else {
    returnKey = Nan::New<v8::String>((char*)key.data(), key.size()).ToLocalChecked();
  }

  v8::Local<v8::Value> returnValue;
  if (iterator->valueAsBuffer) {
    returnValue = Nan::CopyBuffer((char*)value.data(), value.size()).ToLocalChecked();
  } else {
    returnValue = Nan::New<v8::String>((char*)value.data(), value.size()).ToLocalChecked();
  }

  // clean up & handle the next/end state see iterator.cc/checkEndCallback
  localCallback(iterator);

  if (ok) {
    v8::Local<v8::Value> argv[] = {
        Nan::Null()
      , returnKey
      , returnValue
    };
    callback->Call(3, argv);
  } else {
    callback->Call(0, NULL);
  }
}

/** END WORKER **/

EndWorker::EndWorker (
    Iterator* iterator
  , Nan::Callback *callback
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

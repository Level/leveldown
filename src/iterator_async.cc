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
  , v8::Persistent<v8::Function> callback
  , void (*localCallback)(Iterator*)
) : AsyncWorker(NULL, callback)
  , iterator(iterator)
  , localCallback(localCallback)
{};

NextWorker::~NextWorker () {}

void NextWorker::Execute () {
  ok = iterator->IteratorNext(key, value);
  if (!ok)
    status = iterator->IteratorStatus();
}

void NextWorker::HandleOKCallback () {
  v8::Local<v8::Value> returnKey;
  if (iterator->keyAsBuffer) {
    returnKey = v8::Local<v8::Value>::New(
      node::Buffer::New((char*)key.data(), key.size())->handle_
    );
  } else {
    returnKey = v8::String::New((char*)key.data(), key.size());
  }

  v8::Local<v8::Value> returnValue;
  if (iterator->valueAsBuffer) {
    returnValue = v8::Local<v8::Value>::New(
      node::Buffer::New((char*)value.data(), value.size())->handle_
    );
  } else {
    returnValue = v8::String::New((char*)value.data(), value.size());
  }

  // clean up & handle the next/end state see iterator.cc/checkEndCallback
  localCallback(iterator);

  if (ok) {
    v8::Local<v8::Value> argv[] = {
        v8::Local<v8::Value>::New(v8::Null())
      , returnKey
      , returnValue
    };
    LD_RUN_CALLBACK(callback, argv, 3);
  } else {
    LD_RUN_CALLBACK(callback, NULL, 0);
  }
}

/** END WORKER **/

EndWorker::EndWorker (
    Iterator* iterator
  , v8::Persistent<v8::Function> callback
) : AsyncWorker(NULL, callback)
  , iterator(iterator)
{};

EndWorker::~EndWorker () {}

void EndWorker::Execute () {
  iterator->IteratorEnd();
}

void EndWorker::HandleOKCallback () {
  iterator->Release();
  LD_RUN_CALLBACK(callback, NULL, 0);
}

} // namespace leveldown

/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <node.h>

#include "leveldown.h"
#include "database.h"
#include "iterator.h"
#include "batch.h"
#include "leveldown_async.h"

namespace leveldown {

NAN_METHOD(DestroyDB) {
  NanScope();

  char* location = NanFromV8String(args[0].As<v8::Object>());

  NanCallback* callback = new NanCallback(
      v8::Local<v8::Function>::Cast(args[1]));

  DestroyWorker* worker = new DestroyWorker(
      location
    , callback
  );

  NanAsyncQueueWorker(worker);

  NanReturnUndefined();
}

NAN_METHOD(RepairDB) {
  NanScope();

  char* location = NanFromV8String(args[0].As<v8::Object>());

 NanCallback* callback = new NanCallback(
      v8::Local<v8::Function>::Cast(args[1]));

  RepairWorker* worker = new RepairWorker(
      location
    , callback
  );

  NanAsyncQueueWorker(worker);

  NanReturnUndefined();
}

void Init (v8::Handle<v8::Object> target) {
  Database::Init();
  leveldown::Iterator::Init();
  leveldown::Batch::Init();

  v8::Local<v8::Function> leveldown =
      v8::FunctionTemplate::New(LevelDOWN)->GetFunction();

  leveldown->Set(
      NanSymbol("destroy")
    , v8::FunctionTemplate::New(DestroyDB)->GetFunction()
  );

  leveldown->Set(
      NanSymbol("repair")
    , v8::FunctionTemplate::New(RepairDB)->GetFunction()
  );

  target->Set(NanSymbol("leveldown"), leveldown);
}

NODE_MODULE(leveldown, Init)

} // namespace leveldown

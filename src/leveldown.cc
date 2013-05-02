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

v8::Handle<v8::Value> DestroyDB (const v8::Arguments& args) {
  v8::HandleScope scope;

  if (args.Length() < 2) {
    LD_THROW_RETURN(destroy() requires `location` and `callback` arguments)
  }

  if (!args[0]->IsString()) {
    LD_THROW_RETURN(destroy() requires a location string argument)
  }

  if (!args[1]->IsFunction()) {
    LD_THROW_RETURN(destroy() requires a callback function argument)
  }

  char* location = FromV8String(args[0]);

  v8::Persistent<v8::Function> callback = v8::Persistent<v8::Function>::New(
      LD_NODE_ISOLATE_PRE
      v8::Local<v8::Function>::Cast(args[1])
  );

  DestroyWorker* worker = new DestroyWorker(
      location
    , callback
  );

  AsyncQueueWorker(worker);

  return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> RepairDB (const v8::Arguments& args) {
  v8::HandleScope scope;

  if (args.Length() < 2) {
    LD_THROW_RETURN(repair() requires `location` and `callback` arguments)
  }

  if (!args[0]->IsString()) {
    LD_THROW_RETURN(repair() requires a location string argument)
  }

  if (!args[1]->IsFunction()) {
    LD_THROW_RETURN(repair() requires a callback function argument)
  }

  char* location = FromV8String(args[0]);

  v8::Persistent<v8::Function> callback = v8::Persistent<v8::Function>::New(
      LD_NODE_ISOLATE_PRE
      v8::Local<v8::Function>::Cast(args[1])
  );

  RepairWorker* worker = new RepairWorker(
      location
    , callback
  );

  AsyncQueueWorker(worker);

  return scope.Close(v8::Undefined());
}

void Init (v8::Handle<v8::Object> target) {
  Database::Init();
  leveldown::Iterator::Init();
  leveldown::Batch::Init();

  v8::Local<v8::Function> leveldown =
      v8::FunctionTemplate::New(LevelDOWN)->GetFunction();

  leveldown->Set(
      v8::String::NewSymbol("destroy")
    , v8::FunctionTemplate::New(DestroyDB)->GetFunction()
  );

  leveldown->Set(
      v8::String::NewSymbol("repair")
    , v8::FunctionTemplate::New(RepairDB)->GetFunction()
  );

  target->Set(v8::String::NewSymbol("leveldown"), leveldown);
}

NODE_MODULE(leveldown, Init)

} // namespace leveldown

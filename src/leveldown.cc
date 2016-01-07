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
  Nan::HandleScope scope;

  if (info.Length() < 2) {
    return Nan::ThrowError("destroy() requires `location` and `callback` arguments");
  }

  if (!info[0]->IsString()) {
    return Nan::ThrowError("destroy() requires a location string argument");
  }

  if (!info[1]->IsFunction()) {
    return Nan::ThrowError("destroy() requires a callback function argument");
  }

  Nan::Utf8String* location = new Nan::Utf8String(info[0]);

  Nan::Callback* callback = new Nan::Callback(
      v8::Local<v8::Function>::Cast(info[1]));

  DestroyWorker* worker = new DestroyWorker(
      location
    , callback
  );

  Nan::AsyncQueueWorker(worker);
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(RepairDB) {
  Nan::HandleScope scope;

  if (info.Length() < 2) {
    return Nan::ThrowError("repair() requires `location` and `callback` arguments");
  }

  if (!info[0]->IsString()) {
    return Nan::ThrowError("repair() requires a location string argument");
  }

  if (!info[1]->IsFunction()) {
    return Nan::ThrowError("repair() requires a callback function argument");
  }

  Nan::Utf8String* location = new Nan::Utf8String(info[0]);

  Nan::Callback* callback = new Nan::Callback(
      v8::Local<v8::Function>::Cast(info[1]));

  RepairWorker* worker = new RepairWorker(
      location
    , callback
  );

  Nan::AsyncQueueWorker(worker);

  info.GetReturnValue().SetUndefined();
}

void Init (v8::Handle<v8::Object> target) {
  Database::Init();
  leveldown::Iterator::Init();
  leveldown::Batch::Init();

  v8::Local<v8::Function> leveldown =
      Nan::New<v8::FunctionTemplate>(LevelDOWN)->GetFunction();

  leveldown->Set(
      Nan::New("destroy").ToLocalChecked()
    , Nan::New<v8::FunctionTemplate>(DestroyDB)->GetFunction()
  );

  leveldown->Set(
      Nan::New("repair").ToLocalChecked()
    , Nan::New<v8::FunctionTemplate>(RepairDB)->GetFunction()
  );

  target->Set(Nan::New("leveldown").ToLocalChecked(), leveldown);
}

NODE_MODULE(leveldown, Init)

} // namespace leveldown

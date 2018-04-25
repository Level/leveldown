#include <node.h>
#include <node_buffer.h>
#include <nan.h>

#include "database.h"
#include "batch_async.h"
#include "batch.h"
#include "common.h"

namespace leveldown {

static Nan::Persistent<v8::FunctionTemplate> batch_constructor;

Batch::Batch (leveldown::Database* database, bool sync) : database(database) {
  options = new leveldb::WriteOptions();
  options->sync = sync;
  batch = new leveldb::WriteBatch();
  hasData = false;
}

Batch::~Batch () {
  delete options;
  delete batch;
}

leveldb::Status Batch::Write () {
  return database->WriteBatchToDatabase(options, batch);
}

void Batch::Init () {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(Batch::New);
  batch_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New("Batch").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "put", Batch::Put);
  Nan::SetPrototypeMethod(tpl, "del", Batch::Del);
  Nan::SetPrototypeMethod(tpl, "clear", Batch::Clear);
  Nan::SetPrototypeMethod(tpl, "write", Batch::Write);
}

NAN_METHOD(Batch::New) {
  Database* database = Nan::ObjectWrap::Unwrap<Database>(info[0]->ToObject());
  v8::Local<v8::Object> optionsObj;

  if (info.Length() > 1 && info[1]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(info[1]);
  }

  bool sync = BooleanOptionValue(optionsObj, "sync");

  Batch* batch = new Batch(database, sync);
  batch->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Value> Batch::NewInstance (
        v8::Local<v8::Object> database
      , v8::Local<v8::Object> optionsObj
    ) {

  Nan::EscapableHandleScope scope;

  Nan::MaybeLocal<v8::Object> maybeInstance;
  v8::Local<v8::Object> instance;

  v8::Local<v8::FunctionTemplate> constructorHandle =
      Nan::New<v8::FunctionTemplate>(batch_constructor);

  if (optionsObj.IsEmpty()) {
    v8::Local<v8::Value> argv[1] = { database };
    maybeInstance = Nan::NewInstance(constructorHandle->GetFunction(), 1, argv);
  } else {
    v8::Local<v8::Value> argv[2] = { database, optionsObj };
    maybeInstance = Nan::NewInstance(constructorHandle->GetFunction(), 2, argv);
  }

  if (maybeInstance.IsEmpty())
      Nan::ThrowError("Could not create new Batch instance");
  else
    instance = maybeInstance.ToLocalChecked();
  return scope.Escape(instance);
}

NAN_METHOD(Batch::Put) {
  Batch* batch = ObjectWrap::Unwrap<Batch>(info.Holder());
  v8::Local<v8::Function> callback; // purely for the error macros

  v8::Local<v8::Value> keyBuffer = info[0];
  v8::Local<v8::Value> valueBuffer = info[1];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, key)
  LD_STRING_OR_BUFFER_TO_SLICE(value, valueBuffer, value)

  batch->batch->Put(key, value);
  if (!batch->hasData)
    batch->hasData = true;

  DisposeStringOrBufferFromSlice(keyBuffer, key);
  DisposeStringOrBufferFromSlice(valueBuffer, value);

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Batch::Del) {
  Batch* batch = ObjectWrap::Unwrap<Batch>(info.Holder());

  v8::Local<v8::Function> callback; // purely for the error macros

  v8::Local<v8::Value> keyBuffer = info[0];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, key)

  batch->batch->Delete(key);
  if (!batch->hasData)
    batch->hasData = true;

  DisposeStringOrBufferFromSlice(keyBuffer, key);

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Batch::Clear) {
  Batch* batch = ObjectWrap::Unwrap<Batch>(info.Holder());

  batch->batch->Clear();
  batch->hasData = false;

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Batch::Write) {
  Batch* batch = ObjectWrap::Unwrap<Batch>(info.Holder());

  if (batch->hasData) {
    Nan::Callback *callback =
        new Nan::Callback(v8::Local<v8::Function>::Cast(info[0]));
    BatchWriteWorker* worker  = new BatchWriteWorker(batch, callback);
    // persist to prevent accidental GC
    v8::Local<v8::Object> _this = info.This();
    worker->SaveToPersistent("batch", _this);
    Nan::AsyncQueueWorker(worker);
  } else {
    LD_RUN_CALLBACK("leveldown:batch.write",
                    v8::Local<v8::Function>::Cast(info[0]),
                    0, NULL);
  }
}

} // namespace leveldown

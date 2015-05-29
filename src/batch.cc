#include <node.h>
#include <node_buffer.h>
#include <nan.h>

#include "database.h"
#include "batch_async.h"
#include "batch.h"
#include "common.h"

namespace leveldown {

static v8::Persistent<v8::FunctionTemplate> batch_constructor;

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
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(Batch::New);
  NanAssignPersistent(batch_constructor, tpl);
  tpl->SetClassName(NanNew("Batch"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "put", Batch::Put);
  NODE_SET_PROTOTYPE_METHOD(tpl, "del", Batch::Del);
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Batch::Clear);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Batch::Write);
}

NAN_METHOD(Batch::New) {
  NanScope();
  
  assert(args.Length()>0 && "Batch::New expects an Object argument");

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());
  v8::Local<v8::Object> optionsObj;

  bool sync = false;
  if (args.Length() > 1 && args[1]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(args[1]);
    sync = BooleanOptionValue(optionsObj, "sync");
  }

  Batch* batch = new Batch(database, sync);
  batch->Wrap(args.This());

  NanReturnValue(args.This());
}

v8::Handle<v8::Value> Batch::NewInstance (
        v8::Handle<v8::Object> database
      , v8::Handle<v8::Object> optionsObj
    ) {

  NanEscapableScope();

  v8::Local<v8::Object> instance;

  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanNew<v8::FunctionTemplate>(batch_constructor);

  if (optionsObj.IsEmpty()) {
    v8::Handle<v8::Value> argv[1] = { database };
    instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  } else {
    v8::Handle<v8::Value> argv[2] = { database, optionsObj };
    instance = constructorHandle->GetFunction()->NewInstance(2, argv);
  }

  return NanEscapeScope(instance);
}

NAN_METHOD(Batch::Put) {
  NanScope();

  if (args.Length() < 2) {
    return NanThrowError("Batch::Put expects 2 arguments (keyBuffer, valueBuffer)");
  }

  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());
  v8::Handle<v8::Function> callback; // purely for the error macros

  v8::Local<v8::Value> keyBuffer = args[0];
  v8::Local<v8::Value> valueBuffer = args[1];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, key)
  LD_STRING_OR_BUFFER_TO_SLICE(value, valueBuffer, value)

  batch->batch->Put(key, value);
  if (!batch->hasData)
    batch->hasData = true;

  DisposeStringOrBufferFromSlice(keyBuffer, key);
  DisposeStringOrBufferFromSlice(valueBuffer, value);

  NanReturnValue(args.Holder());
}

NAN_METHOD(Batch::Del) {
  NanScope();
  
  if (args.Length() < 1) {
    return NanThrowError("Batch::Del expects an argument (keyBuffer)");
  }

  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  v8::Handle<v8::Function> callback; // purely for the error macros

  v8::Local<v8::Value> keyBuffer = args[0];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, key)

  batch->batch->Delete(key);
  if (!batch->hasData)
    batch->hasData = true;

  DisposeStringOrBufferFromSlice(keyBuffer, key);

  NanReturnValue(args.Holder());
}

NAN_METHOD(Batch::Clear) {
  NanScope();

  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  batch->batch->Clear();
  batch->hasData = false;

  NanReturnValue(args.Holder());
}

NAN_METHOD(Batch::Write) {
  NanScope();
  
  if (args.Length() < 1 || !args[0]->IsFunction()) {
    return NanThrowError("Batch::Write expects a function argument");
  }

  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  if (batch->hasData) {
    NanCallback *callback =
        new NanCallback(v8::Local<v8::Function>::Cast(args[0]));
    BatchWriteWorker* worker  = new BatchWriteWorker(batch, callback);
    // persist to prevent accidental GC
    v8::Local<v8::Object> _this = args.This();
    worker->SaveToPersistent("batch", _this);
    NanAsyncQueueWorker(worker);
  } else {
    LD_RUN_CALLBACK(v8::Local<v8::Function>::Cast(args[0]), 0, NULL);
  }

  NanReturnUndefined();
}

} // namespace leveldown

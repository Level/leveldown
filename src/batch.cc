#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "batch_async.h"
#include "batch.h"

namespace leveldown {

v8::Persistent<v8::Function> Batch::constructor;

Batch::Batch (leveldown::Database* database, bool sync) : database(database) {
  options = new leveldb::WriteOptions();
  options->sync = sync;
  batch = new leveldb::WriteBatch();
  references = new std::vector<Reference>;
}

Batch::~Batch () {
  ClearReferences(references);
  delete options;
  delete batch;
}

leveldb::Status Batch::Write () {
  return database->WriteBatchToDatabase(options, batch);
}

void Batch::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(Batch::New);
  tpl->SetClassName(v8::String::NewSymbol("Batch"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("put")
    , v8::FunctionTemplate::New(Batch::Put)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("del")
    , v8::FunctionTemplate::New(Batch::Del)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("clear")
    , v8::FunctionTemplate::New(Batch::Clear)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("write")
    , v8::FunctionTemplate::New(Batch::Write)->GetFunction()
  );
  constructor = v8::Persistent<v8::Function>::New(
      LD_NODE_ISOLATE_PRE
      tpl->GetFunction());
}

v8::Handle<v8::Value> Batch::New (const v8::Arguments& args) {
  v8::HandleScope scope;

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());
  v8::Local<v8::Object> optionsObj;

  if (args.Length() > 1 && args[1]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(args[1]);
  }

  bool sync = BooleanOptionValue(optionsObj, option_sync);

  Batch* batch = new Batch(database, sync);
  batch->Wrap(args.This());

  return args.This();
}

v8::Handle<v8::Value> Batch::NewInstance (
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

v8::Handle<v8::Value> Batch::Put (const v8::Arguments& args) {
  v8::HandleScope scope;
  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  v8::Handle<v8::Function> callback; // purely for the error macros

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], key)
  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[1], value)

  v8::Local<v8::Value> keyBuffer = args[0];
  v8::Local<v8::Value> valueBuffer = args[1];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, key)
  LD_STRING_OR_BUFFER_TO_SLICE(value, valueBuffer, value)

  batch->references->push_back(Reference(
      v8::Persistent<v8::Value>::New(LD_NODE_ISOLATE_PRE keyBuffer)
    , key
  ));
  batch->references->push_back(Reference(
      v8::Persistent<v8::Value>::New(LD_NODE_ISOLATE_PRE valueBuffer)
    , value
  ));

  batch->batch->Put(key, value);

  return scope.Close(args.Holder());
}

v8::Handle<v8::Value> Batch::Del (const v8::Arguments& args) {
  v8::HandleScope scope;
  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  v8::Handle<v8::Function> callback; // purely for the error macros

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], key)

  v8::Local<v8::Value> keyBuffer = args[0];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, key)

  batch->references->push_back(Reference(
      v8::Persistent<v8::Value>::New(LD_NODE_ISOLATE_PRE keyBuffer)
    , key
  ));

  batch->batch->Delete(key);

  return scope.Close(args.Holder());
}

v8::Handle<v8::Value> Batch::Clear (const v8::Arguments& args) {
  v8::HandleScope scope;
  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());

  batch->batch->Clear();

  return scope.Close(args.Holder());
}

v8::Handle<v8::Value> Batch::Write (const v8::Arguments& args) {
  v8::HandleScope scope;
  
  Batch* batch = ObjectWrap::Unwrap<Batch>(args.Holder());
  if (args.Length() == 0) {
    LD_THROW_RETURN(write() requires a callback argument)
  }
  v8::Persistent<v8::Function> callback = v8::Persistent<v8::Function>::New(
      LD_NODE_ISOLATE_PRE
      v8::Local<v8::Function>::Cast(args[0]));

  BatchWriteWorker* worker  = new BatchWriteWorker(batch, callback);
  AsyncQueueWorker(worker);

  return v8::Undefined();
}

} // namespace leveldown

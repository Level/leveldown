/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License
 * <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <node.h>
#include <node_buffer.h>

#include "leveldb/db.h"
#include "leveldb/write_batch.h"

#include "leveldown.h"
#include "database.h"
#include "async.h"
#include "database_async.h"
#include "cbatch.h"
#include "iterator.h"

namespace leveldown {

Database::Database (char* location) : location(location) {
  db = NULL;
};

Database::~Database () {
  if (db != NULL)
    delete db;
};

const char* Database::Location() const { return location; }

/* Calls from worker threads, NO V8 HERE *****************************/

leveldb::Status Database::OpenDatabase (
        leveldb::Options* options
      , std::string location
    ) {
  return leveldb::DB::Open(*options, location, &db);
}

leveldb::Status Database::PutToDatabase (
        leveldb::WriteOptions* options
      , leveldb::Slice key
      , leveldb::Slice value
    ) {
  return db->Put(*options, key, value);
}

leveldb::Status Database::GetFromDatabase (
        leveldb::ReadOptions* options
      , leveldb::Slice key
      , std::string& value
    ) {
  return db->Get(*options, key, &value);
}

leveldb::Status Database::DeleteFromDatabase (
        leveldb::WriteOptions* options
      , leveldb::Slice key
    ) {
  return db->Delete(*options, key);
}

leveldb::Status Database::WriteBatchToDatabase (
        leveldb::WriteOptions* options
      , leveldb::WriteBatch* batch
    ) {
  return db->Write(*options, batch);
}

uint64_t Database::ApproximateSizeFromDatabase (const leveldb::Range* range) {
  uint64_t size;
  db->GetApproximateSizes(range, 1, &size);
  return size;
}

leveldb::Iterator* Database::NewIterator (leveldb::ReadOptions* options) {
  return db->NewIterator(*options);
}

const leveldb::Snapshot* Database::NewSnapshot () {
  return db->GetSnapshot();
}

void Database::ReleaseSnapshot (const leveldb::Snapshot* snapshot) {
  return db->ReleaseSnapshot(snapshot);
}

void Database::CloseDatabase () {
  delete db;
  db = NULL;
}

/* V8 exposed functions *****************************/

v8::Persistent<v8::Function> Database::constructor;

v8::Handle<v8::Value> LevelDOWN (const v8::Arguments& args) {
  v8::HandleScope scope;
  return scope.Close(Database::NewInstance(args));
}

void Database::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
  tpl->SetClassName(v8::String::NewSymbol("Database"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("open")
    , v8::FunctionTemplate::New(Open)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("close")
    , v8::FunctionTemplate::New(Close)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("put")
    , v8::FunctionTemplate::New(Put)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("get")
    , v8::FunctionTemplate::New(Get)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("del")
    , v8::FunctionTemplate::New(Delete)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("batch")
    , v8::FunctionTemplate::New(Batch)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("approximateSize")
    , v8::FunctionTemplate::New(ApproximateSize)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("iterator")
    , v8::FunctionTemplate::New(Iterator)->GetFunction()
  );
  tpl->PrototypeTemplate()->Set(
      v8::String::NewSymbol("write")
    , v8::FunctionTemplate::New(Write)->GetFunction()
  );
  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
}

v8::Handle<v8::Value> Database::New (const v8::Arguments& args) {
  v8::HandleScope scope;

  if (args.Length() == 0) {
    LD_THROW_RETURN("leveldown() requires at least a location argument")
  }

  if (!args[0]->IsString()) {
    LD_THROW_RETURN("leveldown() requires a location string argument")
  }

  LD_FROM_V8_STRING(location, v8::Handle<v8::String>::Cast(args[0]))

  Database* obj = new Database(location);
  obj->Wrap(args.This());

  return args.This();
}

v8::Handle<v8::Value> Database::NewInstance (const v8::Arguments& args) {
  v8::HandleScope scope;

  v8::Local<v8::Object> instance;

  if (args.Length() == 0) {
    v8::Handle<v8::Value> argv[0];
    instance = constructor->NewInstance(0, argv);
  } else {
    v8::Handle<v8::Value> argv[] = { args[0] };
    instance = constructor->NewInstance(1, argv);
  }

  return scope.Close(instance);
}

v8::Handle<v8::Value> Database::Open (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON(open, 0, 1)

  LD_BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, createIfMissing)
  LD_BOOLEAN_OPTION_VALUE(optionsObj, errorIfExists)
  LD_BOOLEAN_OPTION_VALUE(optionsObj, compression)
  LD_UINT32_OPTION_VALUE(optionsObj, cacheSize, 8 << 20)

  OpenWorker* worker = new OpenWorker(
      database
    , callback
    , createIfMissing
    , errorIfExists
    , compression
    , cacheSize
  );

  AsyncQueueWorker(worker);

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::Close (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON_ONEARG(close)

  CloseWorker* worker = new CloseWorker(database, callback);
  AsyncQueueWorker(worker);

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::Put (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON(put, 2, 3)

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], key)
  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[1], value)

  v8::Local<v8::Value> keyBufferV = args[0];
  v8::Local<v8::Value> valueBufferV = args[1];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBufferV)
  LD_STRING_OR_BUFFER_TO_SLICE(value, valueBufferV)

  v8::Persistent<v8::Value> keyBuffer =
      v8::Persistent<v8::Value>::New(keyBufferV);
  v8::Persistent<v8::Value> valueBuffer =
      v8::Persistent<v8::Value>::New(valueBufferV);

  LD_BOOLEAN_OPTION_VALUE(optionsObj, sync)

  WriteWorker* worker  = new WriteWorker(
      database
    , callback
    , key
    , value
    , sync
    , keyBuffer
    , valueBuffer
  );
  AsyncQueueWorker(worker);

  return v8::Undefined();
}

Handle<Value> Database::Write(const Arguments& args) {
  HandleScope scope;

  Database* database = ObjectWrap::Unwrap<Database>(args.This());

  if (args.Length() < 1) {
    ThrowException(Exception::Error(String::New("batch required")));
    return scope.Close(Undefined());
  }

  WriteParams *params = new WriteParams();
  params->request.data = params;
  params->db = database->db;
  params->cb = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  params->batch = ObjectWrap::Unwrap<CBatch>(args[0]->ToObject());

  uv_queue_work(uv_default_loop(), &params->request, WriteDoing, WriteAfter);

  return scope.Close(args.Holder());
}

void Database::WriteDoing (uv_work_t *req) {
  WriteParams *params = (WriteParams *)req->data;
  leveldb::WriteBatch wb = params->batch->batch;

  params->status = params->db->Write(leveldb::WriteOptions(), &wb);
}

void Database::WriteAfter (uv_work_t *req) {
  HandleScope scope;
  WriteParams *params = (WriteParams *)req->data;

  Handle<Value> argv[1];

  if (params->status.ok()) {
    argv[0] = Local<Value>::New(Undefined());
  } else {
    argv[0] = Local<Value>::New(String::New(params->status.ToString().data()));
  }

  if (params->cb->IsFunction()) {
    TryCatch try_catch;
    params->cb->Call(Context::GetCurrent()->Global(), 1, argv);
    if (try_catch.HasCaught()) {
      FatalException(try_catch);
    }
  }

  params->cb.Dispose();
  delete params;
  scope.Close(Undefined());
}
v8::Handle<v8::Value> Database::Get (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON(put, 1, 2)

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], key)

  v8::Local<v8::Value> keyBufferV = args[0];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBufferV)

  v8::Persistent<v8::Value> keyBuffer = v8::Persistent<v8::Value>::New(keyBufferV);

  LD_BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, asBuffer)
  LD_BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, fillCache)

  ReadWorker* worker = new ReadWorker(
      database
    , callback
    , key
    , asBuffer
    , fillCache
    , keyBuffer
  );
  AsyncQueueWorker(worker);

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::Delete (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON(put, 1, 2)

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], key)

  v8::Local<v8::Value> keyBufferV = args[0];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBufferV)

  v8::Persistent<v8::Value> keyBuffer =
      v8::Persistent<v8::Value>::New(keyBufferV);

  LD_BOOLEAN_OPTION_VALUE(optionsObj, sync)

  DeleteWorker* worker = new DeleteWorker(
      database
    , callback
    , key
    , sync
    , keyBuffer
  );
  AsyncQueueWorker(worker);

  return v8::Undefined();
}

/* property key & value strings for elements of the array sent to batch() */
LD_SYMBOL ( str_key   , key   );
LD_SYMBOL ( str_value , value );
LD_SYMBOL ( str_type  , type  );
LD_SYMBOL ( str_del   , del   );
LD_SYMBOL ( str_put   , put   );

v8::Handle<v8::Value> Database::Batch (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON(batch, 1, 2)

  LD_BOOLEAN_OPTION_VALUE(optionsObj, sync)

  v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(args[0]);

  std::vector< v8::Persistent<v8::Value> >* references =
      new std::vector< v8::Persistent<v8::Value> >;
  leveldb::WriteBatch* batch = new leveldb::WriteBatch();

  for (unsigned int i = 0; i < array->Length(); i++) {
    if (!array->Get(i)->IsObject())
      continue;

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(array->Get(i));

    LD_CB_ERR_IF_NULL_OR_UNDEFINED(obj->Get(str_type), type)

    v8::Local<v8::Value> keyBuffer = obj->Get(str_key);
    LD_CB_ERR_IF_NULL_OR_UNDEFINED(keyBuffer, key)

    if (obj->Get(str_type)->StrictEquals(str_del)) {
      LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer)

      batch->Delete(key);
      if (node::Buffer::HasInstance(keyBuffer->ToObject()))
        references->push_back(v8::Persistent<v8::Value>::New(keyBuffer));
    } else if (obj->Get(str_type)->StrictEquals(str_put)) {
      v8::Local<v8::Value> valueBuffer = obj->Get(str_value);
      LD_CB_ERR_IF_NULL_OR_UNDEFINED(valueBuffer, value)

      LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer)
      LD_STRING_OR_BUFFER_TO_SLICE(value, valueBuffer)

      batch->Put(key, value);
      if (node::Buffer::HasInstance(keyBuffer->ToObject()))
        references->push_back(v8::Persistent<v8::Value>::New(keyBuffer));
      if (node::Buffer::HasInstance(valueBuffer->ToObject()))
        references->push_back(v8::Persistent<v8::Value>::New(valueBuffer));
    }
  }

  AsyncQueueWorker(new BatchWorker(
      database
    , callback
    , batch
    , references
    , sync
  ));

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::ApproximateSize (const v8::Arguments& args) {
  v8::HandleScope scope;

  v8::Local<v8::Value> startBufferV = args[0];
  v8::Local<v8::Value> endBufferV = args[1];

  if (startBufferV->IsNull()
      || startBufferV->IsUndefined()
      || startBufferV->IsFunction() // callback in pos 0?
      || endBufferV->IsNull()
      || endBufferV->IsUndefined()
      || endBufferV->IsFunction() // callback in pos 1?
      ) {
    LD_THROW_RETURN( \
      "approximateSize() requires valid `start`, `end` and `callback` arguments" \
    )
  }

  LD_METHOD_SETUP_COMMON(approximateSize, -1, 2)

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], start)
  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[1], end)

  LD_STRING_OR_BUFFER_TO_SLICE(start, startBufferV)
  LD_STRING_OR_BUFFER_TO_SLICE(end, endBufferV)

  v8::Persistent<v8::Value> startBuffer =
      v8::Persistent<v8::Value>::New(startBufferV);
  v8::Persistent<v8::Value> endBuffer =
      v8::Persistent<v8::Value>::New(endBufferV);

  ApproximateSizeWorker* worker  = new ApproximateSizeWorker(
      database
    , callback
    , start
    , end
    , startBuffer
    , endBuffer
  );
  AsyncQueueWorker(worker);

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::Iterator (const v8::Arguments& args) {
  v8::HandleScope scope;

  v8::Local<v8::Object> optionsObj;
  if (args.Length() > 0 && args[0]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(args[0]);
  }

  return scope.Close(Iterator::NewInstance(args.This(), optionsObj));
}


} // namespace leveldown

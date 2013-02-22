/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License
 * <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <node.h>
#include <node_buffer.h>

#include "leveldb/db.h"

#include "leveldown.h"
#include "database.h"
#include "async.h"
#include "database_async.h"
#include "batch.h"
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
  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
}

v8::Handle<v8::Value> Database::New (const v8::Arguments& args) {
  v8::HandleScope scope;

  if (args.Length() == 0) {
    THROW_RETURN("leveldown() requires at least a location argument")
  }

  if (!args[0]->IsString()) {
    THROW_RETURN("leveldown() requires a location string argument")
  }

  FROM_V8_STRING(location, v8::Handle<v8::String>::Cast(args[0]))

  Database* obj = new Database(location);
  obj->Wrap(args.This());

  return args.This();
}

v8::Handle<v8::Value> Database::NewInstance (const v8::Arguments& args) {
  v8::HandleScope scope;

  v8::Handle<v8::Value> argv[args.Length()];

  for (int i = 0; i < args.Length(); i++)
    argv[i] = args[i];

  v8::Local<v8::Object> instance = constructor->NewInstance(args.Length(), argv);

  return scope.Close(instance);
}

v8::Handle<v8::Value> Database::Open (const v8::Arguments& args) {
  v8::HandleScope scope;

  METHOD_SETUP_COMMON(open, 0, 1)

  BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, createIfMissing)
  BOOLEAN_OPTION_VALUE(optionsObj, errorIfExists)
  BOOLEAN_OPTION_VALUE(optionsObj, compression)
  UINT32_OPTION_VALUE(optionsObj, cacheSize, 8 << 20)

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

  METHOD_SETUP_COMMON_ONEARG(close)

  CloseWorker* worker = new CloseWorker(database, callback);
  AsyncQueueWorker(worker);

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::Put (const v8::Arguments& args) {
  v8::HandleScope scope;

  METHOD_SETUP_COMMON(put, 2, 3)

  CB_ERR_IF_NULL_OR_UNDEFINED(0, Key)
  CB_ERR_IF_NULL_OR_UNDEFINED(1, Value)

  v8::Local<v8::Value> keyBufferV = args[0];
  v8::Local<v8::Value> valueBufferV = args[1];
  STRING_OR_BUFFER_TO_SLICE(key, keyBufferV, Key)
  STRING_OR_BUFFER_TO_SLICE(value, valueBufferV, Value)

  v8::Persistent<v8::Value> keyBuffer = v8::Persistent<v8::Value>::New(keyBufferV);
  v8::Persistent<v8::Value> valueBuffer = v8::Persistent<v8::Value>::New(valueBufferV);
  BOOLEAN_OPTION_VALUE(optionsObj, sync)

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

v8::Handle<v8::Value> Database::Get (const v8::Arguments& args) {
  v8::HandleScope scope;

  METHOD_SETUP_COMMON(put, 1, 2)

  CB_ERR_IF_NULL_OR_UNDEFINED(0, Key)

  v8::Local<v8::Value> keyBufferV = args[0];
  STRING_OR_BUFFER_TO_SLICE(key, keyBufferV, Key)

  v8::Persistent<v8::Value> keyBuffer = v8::Persistent<v8::Value>::New(keyBufferV);

  BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, asBuffer)
  BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, fillCache)

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

  METHOD_SETUP_COMMON(put, 1, 2)

  CB_ERR_IF_NULL_OR_UNDEFINED(0, Key)

  v8::Local<v8::Value> keyBufferV = args[0];
  STRING_OR_BUFFER_TO_SLICE(key, keyBufferV, Key)

  v8::Persistent<v8::Value> keyBuffer = v8::Persistent<v8::Value>::New(keyBufferV);

  BOOLEAN_OPTION_VALUE(optionsObj, sync)

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

v8::Handle<v8::Value> Database::Batch (const v8::Arguments& args) {
  v8::HandleScope scope;

  LU_STR ( key   );
  LU_STR ( value );
  LU_STR ( type  );
  LU_STR ( del   );
  LU_STR ( put   );

  Database* database = node::ObjectWrap::Unwrap<Database>(args.This());
  v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(args[0]);
  v8::Local<v8::Object> optionsObj = v8::Local<v8::Object>::Cast(args[1]);
  BOOLEAN_OPTION_VALUE(optionsObj, sync)
  v8::Persistent<v8::Function> callback =
      v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(args[2]));

  std::vector<BatchOp*>* operations = new std::vector<BatchOp*>;
  for (unsigned int i = 0; i < array->Length(); i++) {
    if (!array->Get(i)->IsObject())
      continue;

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(array->Get(i));
    if (!obj->Has(str_type) && !obj->Has(str_key))
      continue;

    v8::Local<v8::Value> keyBuffer = obj->Get(str_key);

    if (obj->Get(str_type)->StrictEquals(str_del)) {
      STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, Key)
      operations->push_back(new BatchDelete(
          key
        , v8::Persistent<v8::Value>::New(keyBuffer)
      ));
    } else if (obj->Get(str_type)->StrictEquals(str_put) && obj->Has(str_value)) {
      v8::Local<v8::Value> valueBuffer = obj->Get(str_value);
      STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, Key)
      STRING_OR_BUFFER_TO_SLICE(value, valueBuffer, Value)
      operations->push_back(new BatchWrite(
          key
        , value
        , v8::Persistent<v8::Value>::New(keyBuffer)
        , v8::Persistent<v8::Value>::New(valueBuffer)
      ));
    }
  }

  AsyncQueueWorker(new BatchWorker(
      database
    , callback
    , operations
    , sync
  ));

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::ApproximateSize (const v8::Arguments& args) {
  v8::HandleScope scope;

  Database* database = node::ObjectWrap::Unwrap<Database>(args.This());
  v8::Persistent<v8::Function> callback =
      v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(args[2]));

  CB_ERR_IF_NULL_OR_UNDEFINED(0, "start")
  CB_ERR_IF_NULL_OR_UNDEFINED(1, "end")

  v8::Persistent<v8::Value> startBuffer = v8::Persistent<v8::Value>::New(args[0]);
  STRING_OR_BUFFER_TO_SLICE(start, startBuffer, Start)
  v8::Persistent<v8::Value> endBuffer = v8::Persistent<v8::Value>::New(args[1]);
  STRING_OR_BUFFER_TO_SLICE(end, endBuffer, End)

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

} // namespace leveldown

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
#include "batch.h"
#include "iterator.h"

namespace leveldown {

Database::Database (char* location) : location(location) {
  db = NULL;
  currentIteratorId = 0;
  pendingCloseWorker = NULL;
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

void Database::ReleaseIterator (uint32_t id) {
  // called each time an Iterator is End()ed, in the main thread
  // we have to remove our reference to it and if it's the last iterator
  // we have to invoke a pending CloseWorker if there is one
  // if there is a pending CloseWorker it means that we're waiting for
  // iterators to end before we can close them
  iterators.erase(id);
  if (iterators.size() == 0 && pendingCloseWorker != NULL) {
    AsyncQueueWorker((AsyncWorker*)pendingCloseWorker);
    pendingCloseWorker = NULL;
  }
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
  constructor = v8::Persistent<v8::Function>::New(
      LD_NODE_ISOLATE_PRE
      tpl->GetFunction());
}

v8::Handle<v8::Value> Database::New (const v8::Arguments& args) {
  v8::HandleScope scope;

  if (args.Length() == 0) {
    LD_THROW_RETURN(leveldown() requires at least a location argument)
  }

  if (!args[0]->IsString()) {
    LD_THROW_RETURN(leveldown() requires a location string argument)
  }

  char* location = FromV8String(args[0]);

  Database* obj = new Database(location);
  obj->Wrap(args.This());

  return args.This();
}

v8::Handle<v8::Value> Database::NewInstance (const v8::Arguments& args) {
  v8::HandleScope scope;

  v8::Local<v8::Object> instance;

  if (args.Length() == 0) {
    instance = constructor->NewInstance(0, NULL);
  } else {
    v8::Handle<v8::Value> argv[] = { args[0] };
    instance = constructor->NewInstance(1, argv);
  }

  return scope.Close(instance);
}

v8::Handle<v8::Value> Database::Open (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON(open, 0, 1)

  bool createIfMissing = BooleanOptionValueDefTrue(
      optionsObj
    , option_createIfMissing
  );
  bool errorIfExists = BooleanOptionValue(optionsObj, option_errorIfExists);
  bool compression = BooleanOptionValue(optionsObj, option_compression);

  uint32_t cacheSize = UInt32OptionValue(
      optionsObj
    , option_cacheSize
    , 8 << 20
  );
  uint32_t writeBufferSize = UInt32OptionValue(
      optionsObj
    , option_writeBufferSize
    , 4 << 20
  );
  uint32_t blockSize = UInt32OptionValue(
      optionsObj
    , option_blockSize
    , 4096
  );
  uint32_t maxOpenFiles = UInt32OptionValue(
      optionsObj
    , option_maxOpenFiles
    , 1000
  );
  uint32_t blockRestartInterval = UInt32OptionValue(
      optionsObj
    , option_blockRestartInterval
    , 16
  );

  OpenWorker* worker = new OpenWorker(
      database
    , callback
    , createIfMissing
    , errorIfExists
    , compression
    , cacheSize
    , writeBufferSize
    , blockSize
    , maxOpenFiles
    , blockRestartInterval
  );

  AsyncQueueWorker(worker);

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::Close (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON_ONEARG(close)

  CloseWorker* worker = new CloseWorker(database, callback);

  if (database->iterators.size() > 0) {
    // yikes, we still have iterators open! naughty naughty.
    // we have to queue up a CloseWorker and manually close each of them.
    // the CloseWorker will be invoked once they are all cleaned up
    database->pendingCloseWorker = worker;

    for (
        std::map< uint32_t, v8::Persistent<v8::Object> >::iterator it
            = database->iterators.begin()
      ; it != database->iterators.end()
      ; ++it) {

        // for each iterator still open, first check if it's already in
        // the process of ending (ended==true means an async End() is
        // in progress), if not, then we call End() with an empty callback
        // function and wait for it to hit ReleaseIterator() where our
        // CloseWorker will be invoked

        leveldown::Iterator* iterator =
            node::ObjectWrap::Unwrap<leveldown::Iterator>(it->second);

        if (!iterator->ended) {
          v8::Local<v8::Function> end =
              v8::Local<v8::Function>::Cast(it->second->Get(
                  v8::String::NewSymbol("end")));
          v8::Local<v8::Value> argv[] = {
              v8::FunctionTemplate::New()->GetFunction() // empty callback
          };
          v8::TryCatch try_catch;
          end->Call(it->second, 1, argv);
          if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
          }
        }
    }
  } else {
    AsyncQueueWorker(worker);
  }

  return v8::Undefined();
}

v8::Handle<v8::Value> Database::Put (const v8::Arguments& args) {
  v8::HandleScope scope;

  LD_METHOD_SETUP_COMMON(put, 2, 3)

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], key)
  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[1], value)

  v8::Local<v8::Value> keyBufferV = args[0];
  v8::Local<v8::Value> valueBufferV = args[1];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBufferV, key)
  LD_STRING_OR_BUFFER_TO_SLICE(value, valueBufferV, value)

  v8::Persistent<v8::Value> keyBuffer =
      v8::Persistent<v8::Value>::New(LD_NODE_ISOLATE_PRE keyBufferV);
  v8::Persistent<v8::Value> valueBuffer =
      v8::Persistent<v8::Value>::New(LD_NODE_ISOLATE_PRE valueBufferV);

  bool sync = BooleanOptionValue(optionsObj, option_sync);

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

  LD_METHOD_SETUP_COMMON(get, 1, 2)

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], key)

  v8::Local<v8::Value> keyBufferV = args[0];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBufferV, key)

  v8::Persistent<v8::Value> keyBuffer = v8::Persistent<v8::Value>::New(
      LD_NODE_ISOLATE_PRE
      keyBufferV);

  bool asBuffer = BooleanOptionValueDefTrue(optionsObj, option_asBuffer);
  bool fillCache = BooleanOptionValueDefTrue(optionsObj, option_fillCache);

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

  LD_METHOD_SETUP_COMMON(del, 1, 2)

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], key)

  v8::Local<v8::Value> keyBufferV = args[0];
  LD_STRING_OR_BUFFER_TO_SLICE(key, keyBufferV, key)

  v8::Persistent<v8::Value> keyBuffer =
      v8::Persistent<v8::Value>::New(LD_NODE_ISOLATE_PRE keyBufferV);

  bool sync = BooleanOptionValue(optionsObj, option_sync);

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

  if ((args.Length() == 0 || args.Length() == 1) && !args[0]->IsArray()) {
    v8::Local<v8::Object> optionsObj;
    if (args.Length() > 0 && args[0]->IsObject()) {
      optionsObj = v8::Local<v8::Object>::Cast(args[0]);
    }
    return scope.Close(Batch::NewInstance(args.This(), optionsObj));
  }

  LD_METHOD_SETUP_COMMON(batch, 1, 2)

  bool sync = BooleanOptionValue(optionsObj, option_sync);

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
      LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, key)

      batch->Delete(key);
      if (node::Buffer::HasInstance(keyBuffer->ToObject()))
        references->push_back(v8::Persistent<v8::Value>::New(
            LD_NODE_ISOLATE_PRE
            keyBuffer));
    } else if (obj->Get(str_type)->StrictEquals(str_put)) {
      v8::Local<v8::Value> valueBuffer = obj->Get(str_value);
      LD_CB_ERR_IF_NULL_OR_UNDEFINED(valueBuffer, value)

      LD_STRING_OR_BUFFER_TO_SLICE(key, keyBuffer, key)
      LD_STRING_OR_BUFFER_TO_SLICE(value, valueBuffer, value)

      batch->Put(key, value);
      if (node::Buffer::HasInstance(keyBuffer->ToObject()))
        references->push_back(v8::Persistent<v8::Value>::New(
            LD_NODE_ISOLATE_PRE
            keyBuffer));
      if (node::Buffer::HasInstance(valueBuffer->ToObject()))
        references->push_back(v8::Persistent<v8::Value>::New(
            LD_NODE_ISOLATE_PRE
            valueBuffer));
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
    LD_THROW_RETURN(approximateSize() requires valid `start`, `end` and `callback` arguments)
  }

  LD_METHOD_SETUP_COMMON(approximateSize, -1, 2)

  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[0], start)
  LD_CB_ERR_IF_NULL_OR_UNDEFINED(args[1], end)

  LD_STRING_OR_BUFFER_TO_SLICE(start, startBufferV, start)
  LD_STRING_OR_BUFFER_TO_SLICE(end, endBufferV, end)

  v8::Persistent<v8::Value> startBuffer =
      v8::Persistent<v8::Value>::New(LD_NODE_ISOLATE_PRE startBufferV);
  v8::Persistent<v8::Value> endBuffer =
      v8::Persistent<v8::Value>::New(LD_NODE_ISOLATE_PRE endBufferV);

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

  Database* database = node::ObjectWrap::Unwrap<Database>(args.This());

  v8::Local<v8::Object> optionsObj;
  if (args.Length() > 0 && args[0]->IsObject()) {
    optionsObj = v8::Local<v8::Object>::Cast(args[0]);
  }

  // each iterator gets a unique id for this Database, so we can
  // easily store & lookup on our `iterators` map
  uint32_t id = database->currentIteratorId++;
  v8::TryCatch try_catch;
  v8::Handle<v8::Object> iterator = Iterator::NewInstance(
      args.This()
    , v8::Number::New(id)
    , optionsObj
  );
  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }

  // register our iterator
  database->iterators[id] =
      node::ObjectWrap::Unwrap<leveldown::Iterator>(iterator)->handle_;

  return scope.Close(iterator);
}


} // namespace leveldown

/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "leveldown.h"
#include "async.h"
#include "database_async.h"

namespace leveldown {

/** OPEN WORKER **/

OpenWorker::OpenWorker (
    Database* database
  , v8::Persistent<v8::Function> callback
  , bool createIfMissing
  , bool errorIfExists
  , bool compression
  , uint32_t cacheSize
  , uint32_t writeBufferSize
  , uint32_t blockSize
  , uint32_t maxOpenFiles
  , uint32_t blockRestartInterval
) : AsyncWorker(database, callback)
{
  options = new leveldb::Options();
  options->create_if_missing      = createIfMissing;
  options->error_if_exists        = errorIfExists;
  options->compression            = compression
      ? leveldb::kSnappyCompression
      : leveldb::kNoCompression;
  options->block_cache            = leveldb::NewLRUCache(cacheSize);
  options->write_buffer_size      = writeBufferSize;
  options->block_size             = blockSize;
  options->max_open_files         = maxOpenFiles;
  options->block_restart_interval = blockRestartInterval;
};

OpenWorker::~OpenWorker () {
  delete options;
}

void OpenWorker::Execute () {
  status = database->OpenDatabase(options, database->Location());
}

/** CLOSE WORKER **/

CloseWorker::CloseWorker (
    Database* database
  , v8::Persistent<v8::Function> callback
) : AsyncWorker(database, callback)
{};

CloseWorker::~CloseWorker () {}

void CloseWorker::Execute () {
  database->CloseDatabase();
}

void CloseWorker::WorkComplete () {
  v8::HandleScope scope;
  HandleOKCallback();
  callback.Dispose(LD_NODE_ISOLATE);
}

/** IO WORKER (abstract) **/

IOWorker::IOWorker (
    Database* database
  , v8::Persistent<v8::Function> callback
  , leveldb::Slice key
  , v8::Persistent<v8::Value> keyPtr
) : AsyncWorker(database, callback)
  , key(key)
  , keyPtr(keyPtr)
{};

IOWorker::~IOWorker () {}

void IOWorker::WorkComplete () {
  DisposeStringOrBufferFromSlice(keyPtr, key);
  AsyncWorker::WorkComplete();
}

/** READ WORKER **/

ReadWorker::ReadWorker (
    Database* database
  , v8::Persistent<v8::Function> callback
  , leveldb::Slice key
  , bool asBuffer
  , bool fillCache
  , v8::Persistent<v8::Value> keyPtr
) : IOWorker(database, callback, key, keyPtr)
  , asBuffer(asBuffer)
{
  options = new leveldb::ReadOptions();
  options->fill_cache = fillCache;
};

ReadWorker::~ReadWorker () {
  delete options;
}

void ReadWorker::Execute () {
  status = database->GetFromDatabase(options, key, value);
}

void ReadWorker::HandleOKCallback () {
  v8::Local<v8::Value> returnValue;
  if (asBuffer)
    returnValue = v8::Local<v8::Value>::New(
      node::Buffer::New((char*)value.data(), value.size())->handle_
    );
  else
    returnValue = v8::String::New((char*)value.data(), value.size());
  v8::Local<v8::Value> argv[] = {
      v8::Local<v8::Value>::New(v8::Null())
    , returnValue
  };
  LD_RUN_CALLBACK(callback, argv, 2);
}

/** DELETE WORKER **/

DeleteWorker::DeleteWorker (
    Database* database
  , v8::Persistent<v8::Function> callback
  , leveldb::Slice key
  , bool sync
  , v8::Persistent<v8::Value> keyPtr
) : IOWorker(database, callback, key, keyPtr)
{
  options = new leveldb::WriteOptions();
  options->sync = sync;
};

DeleteWorker::~DeleteWorker () {
  delete options;
}

void DeleteWorker::Execute () {
  status = database->DeleteFromDatabase(options, key);
}

/** WRITE WORKER **/

WriteWorker::WriteWorker (
    Database* database
  , v8::Persistent<v8::Function> callback
  , leveldb::Slice key
  , leveldb::Slice value
  , bool sync
  , v8::Persistent<v8::Value> keyPtr
  , v8::Persistent<v8::Value> valuePtr
) : DeleteWorker(database, callback, key, sync, keyPtr)
  , value(value)
  , valuePtr(valuePtr)
{};

WriteWorker::~WriteWorker () {}

void WriteWorker::Execute () {
  status = database->PutToDatabase(options, key, value);
}

void WriteWorker::WorkComplete () {
  DisposeStringOrBufferFromSlice(valuePtr, value);
  IOWorker::WorkComplete();
}

/** BATCH WORKER **/

BatchWorker::BatchWorker (
    Database* database
  , v8::Persistent<v8::Function> callback
  , leveldb::WriteBatch* batch
  , std::vector<Reference>* references
  , bool sync
) : AsyncWorker(database, callback)
  , batch(batch)
  , references(references)
{
  options = new leveldb::WriteOptions();
  options->sync = sync;
};

BatchWorker::~BatchWorker () {
  ClearReferences(references);
  delete options;
}

void BatchWorker::Execute () {
  status = database->WriteBatchToDatabase(options, batch);
}

/** APPROXIMATE SIZE WORKER **/

ApproximateSizeWorker::ApproximateSizeWorker (
    Database* database
  , v8::Persistent<v8::Function> callback
  , leveldb::Slice start
  , leveldb::Slice end
  , v8::Persistent<v8::Value> startPtr
  , v8::Persistent<v8::Value> endPtr
) : AsyncWorker(database, callback)
  , range(start, end)
  , startPtr(startPtr)
  , endPtr(endPtr)
{};

ApproximateSizeWorker::~ApproximateSizeWorker () {}

void ApproximateSizeWorker::Execute () {
  size = database->ApproximateSizeFromDatabase(&range);
}

void ApproximateSizeWorker::WorkComplete() {
  DisposeStringOrBufferFromSlice(startPtr, range.start);
  DisposeStringOrBufferFromSlice(endPtr, range.limit);
  AsyncWorker::WorkComplete();
}

void ApproximateSizeWorker::HandleOKCallback () {
  v8::Local<v8::Value> returnValue = v8::Number::New((double) size);
  v8::Local<v8::Value> argv[] = {
      v8::Local<v8::Value>::New(v8::Null())
    , returnValue
  };
  LD_RUN_CALLBACK(callback, argv, 2);
}

} // namespace leveldown

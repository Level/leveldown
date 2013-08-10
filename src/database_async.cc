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
    Database *database
  , NanCallback *callback
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
  SetStatus(database->OpenDatabase(options, database->Location()));
}

/** CLOSE WORKER **/

CloseWorker::CloseWorker (
    Database *database
  , NanCallback *callback
) : AsyncWorker(database, callback)
{};

CloseWorker::~CloseWorker () {}

void CloseWorker::Execute () {
  database->CloseDatabase();
}

void CloseWorker::WorkComplete () {
  NanScope();

  HandleOKCallback();
}

/** IO WORKER (abstract) **/

IOWorker::IOWorker (
    Database *database
  , NanCallback *callback
  , leveldb::Slice key
  , v8::Local<v8::Object> &keyHandle
) : AsyncWorker(database, callback)
  , key(key)
{
  SavePersistent("key", keyHandle);
};

IOWorker::~IOWorker () {}

void IOWorker::WorkComplete () {
  DisposeStringOrBufferFromSlice(GetFromPersistent("key"), key);
  AsyncWorker::WorkComplete();
}

/** READ WORKER **/

ReadWorker::ReadWorker (
    Database *database
  , NanCallback *callback
  , leveldb::Slice key
  , bool asBuffer
  , bool fillCache
  , v8::Local<v8::Object> &keyHandle
) : IOWorker(database, callback, key, keyHandle)
  , asBuffer(asBuffer)
{
  options = new leveldb::ReadOptions();
  options->fill_cache = fillCache;
  SavePersistent("key", keyHandle);
};

ReadWorker::~ReadWorker () {
  delete options;
}

void ReadWorker::Execute () {
  SetStatus(database->GetFromDatabase(options, key, value));
}

void ReadWorker::HandleOKCallback () {
  NanScope();

  v8::Local<v8::Value> returnValue;
  if (asBuffer) {
    returnValue = NanNewBufferHandle((char*)value.data(), value.size());
  } else {
    returnValue = v8::String::New((char*)value.data(), value.size());
  }
  v8::Local<v8::Value> argv[] = {
      v8::Local<v8::Value>::New(v8::Null())
    , returnValue
  };
  callback->Run(2, argv);
}

/** DELETE WORKER **/

DeleteWorker::DeleteWorker (
    Database *database
  , NanCallback *callback
  , leveldb::Slice key
  , bool sync
  , v8::Local<v8::Object> &keyHandle
) : IOWorker(database, callback, key, keyHandle)
{
  options = new leveldb::WriteOptions();
  options->sync = sync;
  SavePersistent("key", keyHandle);
};

DeleteWorker::~DeleteWorker () {
  delete options;
}

void DeleteWorker::Execute () {
  SetStatus(database->DeleteFromDatabase(options, key));
}

/** WRITE WORKER **/

WriteWorker::WriteWorker (
    Database *database
  , NanCallback *callback
  , leveldb::Slice key
  , leveldb::Slice value
  , bool sync
  , v8::Local<v8::Object> &keyHandle
  , v8::Local<v8::Object> &valueHandle
) : DeleteWorker(database, callback, key, sync, keyHandle)
  , value(value)
{
  SavePersistent("value", valueHandle);
};

WriteWorker::~WriteWorker () {}

void WriteWorker::Execute () {
  SetStatus(database->PutToDatabase(options, key, value));
}

void WriteWorker::WorkComplete () {
  DisposeStringOrBufferFromSlice(GetFromPersistent("value"), value);
  IOWorker::WorkComplete();
}

/** BATCH WORKER **/

BatchWorker::BatchWorker (
    Database *database
  , NanCallback *callback
  , leveldb::WriteBatch* batch
  , std::vector<Reference *>* references
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
  SetStatus(database->WriteBatchToDatabase(options, batch));
}

/** APPROXIMATE SIZE WORKER **/

ApproximateSizeWorker::ApproximateSizeWorker (
    Database *database
  , NanCallback *callback
  , leveldb::Slice start
  , leveldb::Slice end
  , v8::Local<v8::Object> &startHandle
  , v8::Local<v8::Object> &endHandle
) : AsyncWorker(database, callback)
  , range(start, end)
{
  SavePersistent("start", startHandle);
  SavePersistent("end", endHandle);
};

ApproximateSizeWorker::~ApproximateSizeWorker () {}

void ApproximateSizeWorker::Execute () {
  size = database->ApproximateSizeFromDatabase(&range);
}

void ApproximateSizeWorker::WorkComplete() {
  DisposeStringOrBufferFromSlice(GetFromPersistent("start"), range.start);
  DisposeStringOrBufferFromSlice(GetFromPersistent("end"), range.limit);
  AsyncWorker::WorkComplete();
}

void ApproximateSizeWorker::HandleOKCallback () {
  NanScope();

  v8::Local<v8::Value> returnValue = v8::Number::New((double) size);
  v8::Local<v8::Value> argv[] = {
      v8::Local<v8::Value>::New(v8::Null())
    , returnValue
  };
  callback->Run(2, argv);
}

} // namespace leveldown

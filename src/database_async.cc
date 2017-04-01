/* Copyright (c) 2012-2017 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

#include <node.h>
#include <node_buffer.h>

#include <rocksdb/write_batch.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/utilities/leveldb_options.h>

#include "database.h"
#include "leveldown.h"
#include "async.h"
#include "database_async.h"

namespace leveldown {

/** OPEN WORKER **/

OpenWorker::OpenWorker (
    Database *database
  , Nan::Callback *callback
  , rocksdb::Cache* blockCache
  , const rocksdb::FilterPolicy* filterPolicy
  , bool createIfMissing
  , bool errorIfExists
  , bool compression
  , uint32_t writeBufferSize
  , uint32_t blockSize
  , uint32_t maxOpenFiles
  , uint32_t blockRestartInterval
) : AsyncWorker(database, callback)
{
  rocksdb::LevelDBOptions levelOptions;

  levelOptions.block_cache            = blockCache;
  levelOptions.filter_policy          = filterPolicy;
  levelOptions.create_if_missing      = createIfMissing;
  levelOptions.error_if_exists        = errorIfExists;
  levelOptions.compression            = compression
      ? rocksdb::kSnappyCompression
      : rocksdb::kNoCompression;
  levelOptions.write_buffer_size      = writeBufferSize;
  levelOptions.block_size             = blockSize;
  levelOptions.max_open_files         = maxOpenFiles;
  levelOptions.block_restart_interval = blockRestartInterval;

  // rocksdb::Options opts = ConvertOptions(levelOptions);
  // options = &opts;



  options = new rocksdb::Options();
  options->create_if_missing = levelOptions.create_if_missing;
  options->error_if_exists = levelOptions.error_if_exists;
  options->paranoid_checks = levelOptions.paranoid_checks;
  options->env = levelOptions.env;
  options->info_log.reset(levelOptions.info_log);
  options->write_buffer_size = levelOptions.write_buffer_size;
  options->max_open_files = levelOptions.max_open_files;
  options->compression = levelOptions.compression;

  // rocksdb::BlockBasedTableOptions table_options;
  // table_options.block_cache.reset(levelOptions.block_cache);
  // table_options.block_size = levelOptions.block_size;
  // table_options.block_restart_interval = levelOptions.block_restart_interval;
  // table_options.filter_policy.reset(levelOptions.filter_policy);
  // options->table_factory.reset(rocksdb::NewBlockBasedTableFactory(levelOptions));

};

OpenWorker::~OpenWorker () {
  delete options;
}

void OpenWorker::Execute () {
  SetStatus(database->OpenDatabase(options));
}

/** CLOSE WORKER **/

CloseWorker::CloseWorker (
    Database *database
  , Nan::Callback *callback
) : AsyncWorker(database, callback)
{};

CloseWorker::~CloseWorker () {}

void CloseWorker::Execute () {
  database->CloseDatabase();
}

void CloseWorker::WorkComplete () {
  Nan::HandleScope scope;
  HandleOKCallback();
  delete callback;
  callback = NULL;
}

/** IO WORKER (abstract) **/

IOWorker::IOWorker (
    Database *database
  , Nan::Callback *callback
  , rocksdb::Slice key
  , v8::Local<v8::Object> &keyHandle
) : AsyncWorker(database, callback)
  , key(key)
{
  Nan::HandleScope scope;

  SaveToPersistent("key", keyHandle);
};

IOWorker::~IOWorker () {}

void IOWorker::WorkComplete () {
  Nan::HandleScope scope;

  DisposeStringOrBufferFromSlice(GetFromPersistent("key"), key);
  AsyncWorker::WorkComplete();
}

/** READ WORKER **/

ReadWorker::ReadWorker (
    Database *database
  , Nan::Callback *callback
  , rocksdb::Slice key
  , bool asBuffer
  , bool fillCache
  , v8::Local<v8::Object> &keyHandle
) : IOWorker(database, callback, key, keyHandle)
  , asBuffer(asBuffer)
{
  Nan::HandleScope scope;

  options = new rocksdb::ReadOptions();
  options->fill_cache = fillCache;
  SaveToPersistent("key", keyHandle);
};

ReadWorker::~ReadWorker () {
  delete options;
}

void ReadWorker::Execute () {
  SetStatus(database->GetFromDatabase(options, key, value));
}

void ReadWorker::HandleOKCallback () {
  Nan::HandleScope scope;

  v8::Local<v8::Value> returnValue;
  if (asBuffer) {
    //TODO: could use NewBuffer if we carefully manage the lifecycle of `value`
    //and avoid an an extra allocation. We'd have to clean up properly when not OK
    //and let the new Buffer manage the data when OK
    returnValue = Nan::CopyBuffer((char*)value.data(), value.size()).ToLocalChecked();
  } else {
    returnValue = Nan::New<v8::String>((char*)value.data(), value.size()).ToLocalChecked();
  }
  v8::Local<v8::Value> argv[] = {
      Nan::Null()
    , returnValue
  };
  callback->Call(2, argv);
}

/** DELETE WORKER **/

DeleteWorker::DeleteWorker (
    Database *database
  , Nan::Callback *callback
  , rocksdb::Slice key
  , bool sync
  , v8::Local<v8::Object> &keyHandle
) : IOWorker(database, callback, key, keyHandle)
{
  Nan::HandleScope scope;

  options = new rocksdb::WriteOptions();
  options->sync = sync;
  SaveToPersistent("key", keyHandle);
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
  , Nan::Callback *callback
  , rocksdb::Slice key
  , rocksdb::Slice value
  , bool sync
  , v8::Local<v8::Object> &keyHandle
  , v8::Local<v8::Object> &valueHandle
) : DeleteWorker(database, callback, key, sync, keyHandle)
  , value(value)
{
  Nan::HandleScope scope;

  SaveToPersistent("value", valueHandle);
};

WriteWorker::~WriteWorker () { }

void WriteWorker::Execute () {
  SetStatus(database->PutToDatabase(options, key, value));
}

void WriteWorker::WorkComplete () {
  Nan::HandleScope scope;

  DisposeStringOrBufferFromSlice(GetFromPersistent("value"), value);
  IOWorker::WorkComplete();
}

/** BATCH WORKER **/

BatchWorker::BatchWorker (
    Database *database
  , Nan::Callback *callback
  , rocksdb::WriteBatch* batch
  , bool sync
) : AsyncWorker(database, callback)
  , batch(batch)
{
  options = new rocksdb::WriteOptions();
  options->sync = sync;
};

BatchWorker::~BatchWorker () {
  delete batch;
  delete options;
}

void BatchWorker::Execute () {
  SetStatus(database->WriteBatchToDatabase(options, batch));
}

/** APPROXIMATE SIZE WORKER **/

ApproximateSizeWorker::ApproximateSizeWorker (
    Database *database
  , Nan::Callback *callback
  , rocksdb::Slice start
  , rocksdb::Slice end
  , v8::Local<v8::Object> &startHandle
  , v8::Local<v8::Object> &endHandle
) : AsyncWorker(database, callback)
  , range(start, end)
{
  Nan::HandleScope scope;

  SaveToPersistent("start", startHandle);
  SaveToPersistent("end", endHandle);
};

ApproximateSizeWorker::~ApproximateSizeWorker () {}

void ApproximateSizeWorker::Execute () {
  size = database->ApproximateSizeFromDatabase(&range);
}

void ApproximateSizeWorker::WorkComplete() {
  Nan::HandleScope scope;

  DisposeStringOrBufferFromSlice(GetFromPersistent("start"), range.start);
  DisposeStringOrBufferFromSlice(GetFromPersistent("end"), range.limit);
  AsyncWorker::WorkComplete();
}

void ApproximateSizeWorker::HandleOKCallback () {
  Nan::HandleScope scope;

  v8::Local<v8::Value> returnValue = Nan::New<v8::Number>((double) size);
  v8::Local<v8::Value> argv[] = {
      Nan::Null()
    , returnValue
  };
  callback->Call(2, argv);
}

/** COMPACT RANGE WORKER **/

CompactRangeWorker::CompactRangeWorker (
    Database *database
  , Nan::Callback *callback
  , rocksdb::Slice start
  , rocksdb::Slice end
  , v8::Local<v8::Object> &startHandle
  , v8::Local<v8::Object> &endHandle
) : AsyncWorker(database, callback)
{
  Nan::HandleScope scope;

  rangeStart = start;
  rangeEnd = end;

  SaveToPersistent("compactStart", startHandle);
  SaveToPersistent("compactEnd", endHandle);
};

CompactRangeWorker::~CompactRangeWorker () {}

void CompactRangeWorker::Execute () {
  database->CompactRangeFromDatabase(&rangeStart, &rangeEnd);
}

void CompactRangeWorker::WorkComplete() {
  Nan::HandleScope scope;

  DisposeStringOrBufferFromSlice(GetFromPersistent("compactStart"), rangeStart);
  DisposeStringOrBufferFromSlice(GetFromPersistent("compactEnd"), rangeEnd);
  AsyncWorker::WorkComplete();
}

void CompactRangeWorker::HandleOKCallback () {
  Nan::HandleScope scope;

  v8::Local<v8::Value> argv[] = {
      Nan::Null()
  };
  callback->Call(1, argv);
}

} // namespace leveldown

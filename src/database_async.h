/* Copyright (c) 2012-2018 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

#ifndef LD_DATABASE_ASYNC_H
#define LD_DATABASE_ASYNC_H

#include <vector>
#include <node.h>

#include <leveldb/cache.h>

#include "async.h"

namespace leveldown {

class OpenWorker : public AsyncWorker {
public:
  OpenWorker(Database *database,
             Nan::Callback *callback,
             leveldb::Cache* blockCache,
             const leveldb::FilterPolicy* filterPolicy,
             bool createIfMissing,
             bool errorIfExists,
             bool compression,
             uint32_t writeBufferSize,
             uint32_t blockSize,
             uint32_t maxOpenFiles,
             uint32_t blockRestartInterval,
             uint32_t maxFileSize);

  virtual ~OpenWorker();
  virtual void Execute();

private:
  leveldb::Options* options;
};

class CloseWorker : public AsyncWorker {
public:
  CloseWorker(Database *database, Nan::Callback *callback);

  virtual ~CloseWorker() {}
  virtual void Execute();
  virtual void WorkComplete();
};

class IOWorker : public AsyncWorker {
public:
  IOWorker(Database *database,
           Nan::Callback *callback,
           const char *resource_name,
           leveldb::Slice key,
           v8::Local<v8::Object> &keyHandle);

  virtual ~IOWorker() {}
  virtual void WorkComplete();

protected:
  leveldb::Slice key;
};

class ReadWorker : public IOWorker {
public:
  ReadWorker(Database *database,
             Nan::Callback *callback,
             leveldb::Slice key,
             bool asBuffer,
             bool fillCache,
             v8::Local<v8::Object> &keyHandle);

  virtual ~ReadWorker();
  virtual void Execute();
  virtual void HandleOKCallback();

private:
  bool asBuffer;
  leveldb::ReadOptions* options;
  std::string value;
};

class DeleteWorker : public IOWorker {
public:
  DeleteWorker(Database *database,
               Nan::Callback *callback,
               leveldb::Slice key,
               bool sync,
               v8::Local<v8::Object> &keyHandle,
               const char *resource_name = "leveldown:db.del");

  virtual ~DeleteWorker();
  virtual void Execute();

protected:
  leveldb::WriteOptions* options;
};

class WriteWorker : public DeleteWorker {
public:
  WriteWorker(Database *database,
              Nan::Callback *callback,
              leveldb::Slice key,
              leveldb::Slice value,
              bool sync,
              v8::Local<v8::Object> &keyHandle,
              v8::Local<v8::Object> &valueHandle);

  virtual ~WriteWorker() {}
  virtual void Execute();
  virtual void WorkComplete();

private:
  leveldb::Slice value;
};

class BatchWorker : public AsyncWorker {
public:
  BatchWorker(Database *database,
              Nan::Callback *callback,
              leveldb::WriteBatch* batch,
              bool sync);

  virtual ~BatchWorker();
  virtual void Execute();

private:
  leveldb::WriteOptions* options;
  leveldb::WriteBatch* batch;
};

class ApproximateSizeWorker : public AsyncWorker {
public:
  ApproximateSizeWorker(Database *database,
                        Nan::Callback *callback,
                        leveldb::Slice start,
                        leveldb::Slice end,
                        v8::Local<v8::Object> &startHandle,
                        v8::Local<v8::Object> &endHandle);

  virtual ~ApproximateSizeWorker() {}
  virtual void Execute();
  virtual void HandleOKCallback();
  virtual void WorkComplete();

  private:
    leveldb::Range range;
    uint64_t size;
};

class CompactRangeWorker : public AsyncWorker {
public:
  CompactRangeWorker(Database *database,
                     Nan::Callback *callback,
                     leveldb::Slice start,
                     leveldb::Slice end,
                     v8::Local<v8::Object> &startHandle,
                     v8::Local<v8::Object> &endHandle);

  virtual ~CompactRangeWorker() {}
  virtual void Execute();
  virtual void HandleOKCallback();
  virtual void WorkComplete();

  private:
    leveldb::Slice rangeStart;
    leveldb::Slice rangeEnd;
};


} // namespace leveldown

#endif

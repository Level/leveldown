/* Copyright (c) 2012-2017 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

#ifndef LD_DATABASE_H
#define LD_DATABASE_H

#include <map>
#include <vector>
#include <node.h>

#include <rocksdb/cache.h>
#include <rocksdb/db.h>
#include <rocksdb/filter_policy.h>
#include <nan.h>

#include "leveldown.h"
#include "iterator.h"

namespace leveldown {

NAN_METHOD(LevelDOWN);

struct Reference {
  Nan::Persistent<v8::Object> handle;
  rocksdb::Slice slice;

  Reference(v8::Local<v8::Value> obj, rocksdb::Slice slice) : slice(slice) {
    v8::Local<v8::Object> _obj = Nan::New<v8::Object>();
    _obj->Set(Nan::New("obj").ToLocalChecked(), obj);
    handle.Reset(_obj);
  };
};

static inline void ClearReferences (std::vector<Reference *> *references) {
  for (std::vector<Reference *>::iterator it = references->begin()
      ; it != references->end()
      ; ) {
    DisposeStringOrBufferFromSlice((*it)->handle, (*it)->slice);
    it = references->erase(it);
  }
  delete references;
}

class Database : public Nan::ObjectWrap {
public:
  static void Init ();
  static v8::Local<v8::Value> NewInstance (v8::Local<v8::String> &location);

  rocksdb::Status OpenDatabase (rocksdb::Options* options);
  rocksdb::Status PutToDatabase (
      rocksdb::WriteOptions* options
    , rocksdb::Slice key
    , rocksdb::Slice value
  );
  rocksdb::Status GetFromDatabase (
      rocksdb::ReadOptions* options
    , rocksdb::Slice key
    , std::string& value
  );
  rocksdb::Status DeleteFromDatabase (
      rocksdb::WriteOptions* options
    , rocksdb::Slice key
  );
  rocksdb::Status WriteBatchToDatabase (
      rocksdb::WriteOptions* options
    , rocksdb::WriteBatch* batch
  );
  uint64_t ApproximateSizeFromDatabase (const rocksdb::Range* range);
  void CompactRangeFromDatabase (const rocksdb::Slice* start, const rocksdb::Slice* end);
  void GetPropertyFromDatabase (const rocksdb::Slice& property, std::string* value);
  rocksdb::Iterator* NewIterator (rocksdb::ReadOptions* options);
  const rocksdb::Snapshot* NewSnapshot ();
  void ReleaseSnapshot (const rocksdb::Snapshot* snapshot);
  void CloseDatabase ();
  void ReleaseIterator (uint32_t id);

  Database (const v8::Local<v8::Value>& from);
  ~Database ();

private:
  Nan::Utf8String* location;
  rocksdb::DB* db;
  uint32_t currentIteratorId;
  void(*pendingCloseWorker);
  rocksdb::Cache* blockCache;
  const rocksdb::FilterPolicy* filterPolicy;

  std::map< uint32_t, leveldown::Iterator * > iterators;

  static void WriteDoing(uv_work_t *req);
  static void WriteAfter(uv_work_t *req);

  static NAN_METHOD(New);
  static NAN_METHOD(Open);
  static NAN_METHOD(Close);
  static NAN_METHOD(Put);
  static NAN_METHOD(Delete);
  static NAN_METHOD(Get);
  static NAN_METHOD(Batch);
  static NAN_METHOD(Write);
  static NAN_METHOD(Iterator);
  static NAN_METHOD(ApproximateSize);
  static NAN_METHOD(CompactRange);
  static NAN_METHOD(GetProperty);
};

} // namespace leveldown

#endif

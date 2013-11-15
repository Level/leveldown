/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LD_DATABASE_H
#define LD_DATABASE_H

#include <map>
#include <vector>
#include <node.h>

#include "rocksdb/db.h"
#include "nan.h"
#include "leveldown.h"
#include "iterator.h"

namespace leveldown {

NAN_METHOD(LevelDOWN);

struct Reference {
  v8::Persistent<v8::Object> handle;
  rocksdb::Slice slice;

  Reference(v8::Local<v8::Value> obj, rocksdb::Slice slice) : slice(slice) {
    v8::Local<v8::Object> _obj = v8::Object::New();
    _obj->Set(NanSymbol("obj"), obj);
    NanAssignPersistent(v8::Object, handle, _obj);
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

class Database : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Handle<v8::Value> NewInstance (v8::Local<v8::String> &location);

  rocksdb::Status OpenDatabase (rocksdb::Options* options, std::string location);
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
  void GetPropertyFromDatabase (const rocksdb::Slice& property, std::string* value);
  rocksdb::Iterator* NewIterator (rocksdb::ReadOptions* options);
  const rocksdb::Snapshot* NewSnapshot ();
  void ReleaseSnapshot (const rocksdb::Snapshot* snapshot);
  void CloseDatabase ();
  const char* Location() const;
  void ReleaseIterator (uint32_t id);

  Database (char* location);
  ~Database ();

private:
  rocksdb::DB* db;
  char* location;
  uint32_t currentIteratorId;
  void(*pendingCloseWorker);

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
  static NAN_METHOD(GetProperty);
};

} // namespace leveldown

#endif

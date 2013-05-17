/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LD_DATABASE_H
#define LD_DATABASE_H

#include <map>
#include <vector>
#include <node.h>

#include "leveldb/db.h"

#include "leveldown.h"

namespace leveldown {

LD_SYMBOL ( option_createIfMissing , createIfMissing ); // for open()
LD_SYMBOL ( option_errorIfExists   , errorIfExists   ); // for open()
LD_SYMBOL ( option_compression     , compression     ); // for open()
LD_SYMBOL ( option_cacheSize       , cacheSize       ); // for open() 
LD_SYMBOL ( option_writeBufferSize , writeBufferSize ); // for open() 
LD_SYMBOL ( option_blockSize       , blockSize       ); // for open() 
LD_SYMBOL ( option_maxOpenFiles    , maxOpenFiles    ); // for open() 
LD_SYMBOL ( option_blockRestartInterval , blockRestartInterval ); // for open() 
LD_SYMBOL ( option_sync            , sync            ); // for put() and delete()
LD_SYMBOL ( option_asBuffer        , asBuffer        ); // for get()
LD_SYMBOL ( option_fillCache       , fillcache       ); // for get() and readStream()

struct AsyncDescriptor;

v8::Handle<v8::Value> LevelDOWN (const v8::Arguments& args);

struct Reference {
  v8::Persistent<v8::Value> ptr;
  leveldb::Slice slice;
  Reference(v8::Persistent<v8::Value> ptr, leveldb::Slice slice) :
      ptr(ptr)
    , slice(slice) { };
};

static inline void ClearReferences (std::vector<Reference>* references) {
  for (std::vector<Reference>::iterator it = references->begin()
      ; it != references->end()
      ; ) {
    DisposeStringOrBufferFromSlice(it->ptr, it->slice);
    it = references->erase(it);
  }
  delete references;
}

class Database : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Handle<v8::Value> NewInstance (const v8::Arguments& args);

  leveldb::Status OpenDatabase (leveldb::Options* options, std::string location);
  leveldb::Status PutToDatabase (
      leveldb::WriteOptions* options
    , leveldb::Slice key
    , leveldb::Slice value
  );
  leveldb::Status GetFromDatabase (
      leveldb::ReadOptions* options
    , leveldb::Slice key
    , std::string& value
  );
  leveldb::Status DeleteFromDatabase (
      leveldb::WriteOptions* options
    , leveldb::Slice key
  );
  leveldb::Status WriteBatchToDatabase (
      leveldb::WriteOptions* options
    , leveldb::WriteBatch* batch
  );
  uint64_t ApproximateSizeFromDatabase (const leveldb::Range* range);
  leveldb::Iterator* NewIterator (leveldb::ReadOptions* options);
  const leveldb::Snapshot* NewSnapshot ();
  void ReleaseSnapshot (const leveldb::Snapshot* snapshot);
  void CloseDatabase ();
  const char* Location() const;
  void ReleaseIterator (uint32_t id);

  Database (char* location);
  ~Database ();

private:
  leveldb::DB* db;
  char* location;
  uint32_t currentIteratorId;
  void(*pendingCloseWorker);

  std::map< uint32_t, v8::Persistent<v8::Object> > iterators;

  static v8::Persistent<v8::Function> constructor;
  static void WriteDoing(uv_work_t *req);
  static void WriteAfter(uv_work_t *req);

  LD_V8_METHOD( New      )
  LD_V8_METHOD( Open     )
  LD_V8_METHOD( Close    )
  LD_V8_METHOD( Put      )
  LD_V8_METHOD( Delete   )
  LD_V8_METHOD( Get      )
  LD_V8_METHOD( Batch    )
  LD_V8_METHOD( Write    )
  LD_V8_METHOD( Iterator )
  LD_V8_METHOD( ApproximateSize )
};

} // namespace leveldown

#endif

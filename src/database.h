/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LU_DATABASE_H
#define LU_DATABASE_H

#include <node.h>

#include "leveldb/db.h"

#include "leveldown.h"

BEGIN_NAMESPACE(leveldown)

LU_SYMBOL ( option_createIfMissing , createIfMissing ); // for open()
LU_SYMBOL ( option_errorIfExists   , errorIfExists   ); // for open()
LU_SYMBOL ( option_compression     , compression     ); // for open()
LU_SYMBOL ( option_cacheSize       , cacheSize       ); // for open() 
LU_SYMBOL ( option_sync            , sync            ); // for put() and delete()
LU_SYMBOL ( option_asBuffer        , asBuffer        ); // for get()
LU_SYMBOL ( option_fillCache       , fillcache       ); // for get() and readStream()

struct AsyncDescriptor;

v8::Handle<v8::Value> LevelDOWN (const v8::Arguments& args);

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

private:
  Database (char* location);
  ~Database ();

  leveldb::DB* db;
  char* location;

  static v8::Persistent<v8::Function> constructor;

  LU_V8_METHOD( New      )
  LU_V8_METHOD( Open     )
  LU_V8_METHOD( Close    )
  LU_V8_METHOD( Put      )
  LU_V8_METHOD( Delete   )
  LU_V8_METHOD( Get      )
  LU_V8_METHOD( Batch    )
  LU_V8_METHOD( Iterator )
  LU_V8_METHOD( ApproximateSize )
};

END_NAMESPACE(leveldown)

#endif

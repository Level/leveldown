#ifndef LD_BATCH_H
#define LD_BATCH_H

#include <vector>
#include <node.h>

#include <leveldb/write_batch.h>

#include "database.h"

namespace leveldown {

class Batch : public node::ObjectWrap {
public:
  static void Init();
  static v8::Handle<v8::Value> NewInstance (
      v8::Handle<v8::Object> database
    , v8::Handle<v8::Object> optionsObj
  );

  Batch  (leveldown::Database* database, bool sync);
  ~Batch ();
  leveldb::Status Write ();

private:
  leveldown::Database* database;
  leveldb::WriteOptions* options;
  leveldb::WriteBatch* batch;
  std::vector<Reference>* references;

  static v8::Persistent<v8::Function> constructor;

  LD_V8_METHOD( New   )
  LD_V8_METHOD( Put   )
  LD_V8_METHOD( Del   )
  LD_V8_METHOD( Clear )
  LD_V8_METHOD( Write )
};

} // namespace leveldown

#endif

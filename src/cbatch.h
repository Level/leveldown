#ifndef BATCH
#define BATCH

#include <node.h>
#include <leveldb/write_batch.h>

using namespace v8;
using namespace node;

class CBatch : public ObjectWrap {
public:
  static void Initialize(v8::Handle<v8::Object> target);
  leveldb::WriteBatch batch;
private:
  CBatch();
  ~CBatch();
  static Persistent<FunctionTemplate> constructor;

  static Handle<Value> New(const Arguments &args);

  static Handle<Value> Put(const Arguments &args);
  static Handle<Value> Del(const Arguments &args);
  static Handle<Value> Clear(const Arguments &args);

};

#endif

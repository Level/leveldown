/* Copyright (c) 2012-2015 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE.md>
 */

#ifndef LD_SNAPSHOT_H
#define LD_SNAPSHOT_H

#include <node.h>
#include <vector>
#include <nan.h>

#include "leveldown.h"
#include "database.h"
#include "async.h"

namespace leveldown {

class Database;
class AsyncWorker;

class Snapshot : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Local<v8::Object> NewInstance (
      v8::Local<v8::Object> database
  );
  static bool HasInstance (v8::Handle<v8::Value> value);

  Snapshot (
      Database* database
  );

  ~Snapshot ();

  void Release ();

  Database* database;
  const leveldb::Snapshot* dbSnapshot;

private:
  v8::Persistent<v8::Object> persistentHandle;
  static NAN_METHOD(New);
};

} // namespace leveldown

#endif

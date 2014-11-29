/* Copyright (c) 2012-2014 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE.md>
 */

#ifndef LD_ITERATOR_H
#define LD_ITERATOR_H

#include <node.h>
#include <vector>
#include <nan.h>

#include "leveldown.h"
#include "database.h"
#include "async.h"

namespace leveldown {

class Database;
class AsyncWorker;

class Iterator : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Local<v8::Object> NewInstance (
      v8::Local<v8::Object> database
    , v8::Local<v8::Number> id
    , v8::Local<v8::Object> optionsObj
  );

  Iterator (
      Database* database
    , uint32_t id
    , leveldb::Slice* start
    , std::string end
    , bool skipStart
    , bool skipEnd
    , bool noReverse
    , bool keys
    , bool values
    , int limit
    , bool fillCache
    , bool keyAsBuffer
    , bool valueAsBuffer
    , v8::Local<v8::Object> &startHandle
    , size_t highWaterMark
  );

  ~Iterator ();

  bool IteratorNext (std::vector<std::pair<std::string, std::string> >& results);
  bool IteratorNext2 (std::vector<std::pair<std::string, std::string> >& results);
  leveldb::Status IteratorStatus ();
  void IteratorEnd ();
  void Release ();

private:
  Database* database;
  uint32_t id;
  leveldb::Iterator* dbIterator;
  leveldb::ReadOptions options;
  leveldb::Slice* start;
  std::string end;
  bool skipStart;
  bool skipEnd;
  bool noReverse;
  int limit;
  size_t highWaterMark;

public:
  bool keys;
  bool values;
  bool keyAsBuffer;
  bool valueAsBuffer;
  bool nexting;
  bool ended;
  int count;
  AsyncWorker* endWorker;

private:
  v8::Persistent<v8::Object> persistentHandle;

  bool Read (std::string& key, std::string& value);
  bool GetIterator ();

  static NAN_METHOD(New);
  static NAN_METHOD(Next);
  static NAN_METHOD(NextSync);
  static NAN_METHOD(End);
  static NAN_METHOD(EndSync);
};

} // namespace leveldown

#endif

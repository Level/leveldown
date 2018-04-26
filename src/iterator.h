/* Copyright (c) 2012-2018 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
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

class Iterator : public Nan::ObjectWrap {
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
    , std::string* end
    , bool reverse
    , bool keys
    , bool values
    , int limit
    , std::string* lt
    , std::string* lte
    , std::string* gt
    , std::string* gte
    , bool fillCache
    , bool keyAsBuffer
    , bool valueAsBuffer
    , size_t highWaterMark
  );

  ~Iterator ();

  bool IteratorNext (std::vector<std::pair<std::string, std::string> >& result);
  leveldb::Status IteratorStatus ();
  void IteratorEnd ();
  void Release ();
  void ReleaseTarget ();

private:
  Database* database;
  uint32_t id;
  leveldb::Iterator* dbIterator;
  leveldb::ReadOptions* options;
  leveldb::Slice* start;
  leveldb::Slice* target;
  std::string* end;
  bool seeking;
  bool landed;
  bool reverse;
  bool keys;
  bool values;
  int limit;
  std::string* lt;
  std::string* lte;
  std::string* gt;
  std::string* gte;
  int count;
  size_t highWaterMark;

public:
  bool keyAsBuffer;
  bool valueAsBuffer;
  bool nexting;
  bool ended;
  AsyncWorker* endWorker;

private:
  bool Read (std::string& key, std::string& value);
  bool GetIterator ();
  bool OutOfRange (leveldb::Slice* target);

  static NAN_METHOD(New);
  static NAN_METHOD(Seek);
  static NAN_METHOD(Next);
  static NAN_METHOD(End);
};

} // namespace leveldown

#endif

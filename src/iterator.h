/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LD_ITERATOR_H
#define LD_ITERATOR_H

#include <node.h>

#include "leveldown.h"
#include "database.h"
#include "async.h"

namespace leveldown {

LD_SYMBOL ( option_start         , start         );
LD_SYMBOL ( option_end           , end           );
LD_SYMBOL ( option_limit         , limit         );
LD_SYMBOL ( option_reverse       , reverse       );
LD_SYMBOL ( option_keys          , keys          );
LD_SYMBOL ( option_values        , values        );
LD_SYMBOL ( option_keyAsBuffer   , keyAsBuffer   );
LD_SYMBOL ( option_valueAsBuffer , valueAsBuffer );

v8::Handle<v8::Value> CreateIterator (const v8::Arguments& args);

class Iterator : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Handle<v8::Object> NewInstance (
      v8::Handle<v8::Object> database
    , v8::Handle<v8::Number> id
    , v8::Handle<v8::Object> optionsObj
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
    , bool fillCache
    , bool keyAsBuffer
    , bool valueAsBuffer
    , v8::Persistent<v8::Value> startPtr
  );

  ~Iterator ();

  bool IteratorNext (std::string& key, std::string& value);
  leveldb::Status IteratorStatus ();
  void IteratorEnd ();
  void Release ();

private:
  Database* database;
  uint32_t id;
  leveldb::Iterator* dbIterator;
  leveldb::ReadOptions* options;
  leveldb::Slice* start;
  std::string* end;
  bool reverse;
  bool keys;
  bool values;
  int limit;
  int count;

public:
  bool keyAsBuffer;
  bool valueAsBuffer;
  bool nexting;
  bool ended;
  AsyncWorker* endWorker;

private:
  v8::Persistent<v8::Value> startPtr;

  bool GetIterator ();

  static v8::Persistent<v8::Function> constructor;

  LD_V8_METHOD( New  )
  LD_V8_METHOD( Next )
  LD_V8_METHOD( End  )
};

} // namespace leveldown

#endif

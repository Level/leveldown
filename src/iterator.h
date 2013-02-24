/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LU_ITERATOR_H
#define LU_ITERATOR_H

#include <node.h>

#include "leveldown.h"
#include "database.h"
#include "async.h"

BEGIN_NAMESPACE(leveldown)

LU_SYMBOL ( option_start         , start         );
LU_SYMBOL ( option_end           , end           );
LU_SYMBOL ( option_limit         , limit         );
LU_SYMBOL ( option_reverse       , reverse       );
LU_SYMBOL ( option_keys          , keys          );
LU_SYMBOL ( option_values        , values        );
LU_SYMBOL ( option_keyAsBuffer   , keyAsBuffer   );
LU_SYMBOL ( option_valueAsBuffer , valueAsBuffer );

v8::Handle<v8::Value> CreateIterator (const v8::Arguments& args);

class Iterator : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Handle<v8::Value> NewInstance (
      v8::Handle<v8::Object> database
    , v8::Handle<v8::Object> optionsObj
  );

  bool IteratorNext (std::string& key, std::string& value);
  leveldb::Status IteratorStatus ();
  void IteratorEnd ();

  Iterator (
      Database* database
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

private:
  Database* database;
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

  LU_V8_METHOD( New  )
  LU_V8_METHOD( Next )
  LU_V8_METHOD( End  )
};

END_NAMESPACE(leveldown)

#endif

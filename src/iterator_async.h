/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LU_ITERATOR_ASYNC_H
#define LU_ITERATOR_ASYNC_H

#include <cstdlib>
#include <vector>
#include <node.h>
#include "async.h"
#include "iterator.h"

using namespace std;
using namespace v8;
using namespace leveldb;

class NextWorker : public AsyncWorker {
public:
  NextWorker (
      leveldown::Iterator* iterator
    , Persistent<Function> dataCallback
    , Persistent<Function> endCallback
  );

  virtual ~NextWorker ();
  virtual void Execute ();
  virtual void HandleOKCallback ();

private:
  leveldown::Iterator* iterator;
  Persistent<Function> endCallback;
  string key;
  string value;
  bool ok;
};

class EndWorker : public AsyncWorker {
public:
  EndWorker (
      leveldown::Iterator* iterator
    , Persistent<Function> endCallback
  );

  virtual ~EndWorker ();
  virtual void Execute ();

private:
  leveldown::Iterator* iterator;
};

#endif

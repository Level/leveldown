/* Copyright (c) 2012-2014 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE.md>
 */

#ifndef LD_LEVELDOWN_ASYNC_H
#define LD_LEVELDOWN_ASYNC_H

#include <node.h>

#include "async.h"

namespace leveldown {

class DestroyWorker : public AsyncWorker {
public:
  DestroyWorker (
      char* location
    , NanCallback *callback
  );

  virtual ~DestroyWorker ();
  virtual void Execute ();

private:
  char* location;
};

class RepairWorker : public AsyncWorker {
public:
  RepairWorker (
      char* location
    , NanCallback *callback
  );

  virtual ~RepairWorker ();
  virtual void Execute ();

private:
  char* location;
};

} // namespace leveldown

#endif

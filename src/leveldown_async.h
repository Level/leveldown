/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LD_LEVELDOWN_ASYNC_H
#define LD_LEVELDOWN_ASYNC_H

#include <node.h>

#include "async.h"

namespace leveldown {

class DestroyWorker : public AsyncWorker {
public:
  DestroyWorker (
      Nan::Utf8String* location
    , Nan::Callback *callback
  );

  virtual ~DestroyWorker ();
  virtual void Execute ();

private:
  Nan::Utf8String* location;
};

class RepairWorker : public AsyncWorker {
public:
  RepairWorker (
      Nan::Utf8String* location
    , Nan::Callback *callback
  );

  virtual ~RepairWorker ();
  virtual void Execute ();

private:
  Nan::Utf8String* location;
};

} // namespace leveldown

#endif

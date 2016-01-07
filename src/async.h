/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LD_ASYNC_H
#define LD_ASYNC_H

#include <node.h>
#include "nan.h"
#include "database.h"

namespace leveldown {

class Database;

/* abstract */ class AsyncWorker : public Nan::AsyncWorker {
public:
  AsyncWorker (
      leveldown::Database* database
    , Nan::Callback *callback
  ) : Nan::AsyncWorker(callback), database(database) {}

protected:
  void SetStatus(leveldb::Status status) {
    this->status = status;
    if (!status.ok())
      SetErrorMessage(status.ToString().c_str());
  }
  Database* database;
private:
  leveldb::Status status;
};

} // namespace leveldown

#endif

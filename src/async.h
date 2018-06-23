#ifndef LD_ASYNC_H
#define LD_ASYNC_H

#include <node.h>
#include <nan.h>
#include "database.h"

namespace leveldown {

class Database;

/* abstract */ class AsyncWorker : public Nan::AsyncWorker {
public:
  AsyncWorker(leveldown::Database* database,
              Nan::Callback *callback,
              const char *resource_name)
    : Nan::AsyncWorker(callback, resource_name), database(database) {}

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

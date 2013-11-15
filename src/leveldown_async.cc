/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <rocksdb/db.h>

#include "leveldown.h"
#include "leveldown_async.h"

namespace leveldown {

/** DESTROY WORKER **/

DestroyWorker::DestroyWorker (
    char* location
  , NanCallback *callback
) : AsyncWorker(NULL, callback)
  , location(location)
{};

DestroyWorker::~DestroyWorker () {
  delete[] location;
}

void DestroyWorker::Execute () {
  rocksdb::Options options;
  SetStatus(rocksdb::DestroyDB(location, options));
}

/** REPAIR WORKER **/

RepairWorker::RepairWorker (
    char* location
  , NanCallback *callback
) : AsyncWorker(NULL, callback)
  , location(location)
{};

RepairWorker::~RepairWorker () {
  delete[] location;
}

void RepairWorker::Execute () {
  rocksdb::Options options;
  SetStatus(rocksdb::RepairDB(location, options));
}

} // namespace leveldown

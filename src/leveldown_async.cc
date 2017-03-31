/* Copyright (c) 2012-2017 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

#include <rocksdb/db.h>

#include "leveldown.h"
#include "leveldown_async.h"

namespace leveldown {

/** DESTROY WORKER **/

DestroyWorker::DestroyWorker (
    Nan::Utf8String* location
  , Nan::Callback *callback
) : AsyncWorker(NULL, callback)
  , location(location)
{};

DestroyWorker::~DestroyWorker () {
  delete location;
}

void DestroyWorker::Execute () {
  rocksdb::Options options;
  SetStatus(rocksdb::DestroyDB(**location, options));
}

/** REPAIR WORKER **/

RepairWorker::RepairWorker (
    Nan::Utf8String* location
  , Nan::Callback *callback
) : AsyncWorker(NULL, callback)
  , location(location)
{};

RepairWorker::~RepairWorker () {
  delete location;
}

void RepairWorker::Execute () {
  rocksdb::Options options;
  SetStatus(rocksdb::RepairDB(**location, options));
}

} // namespace leveldown

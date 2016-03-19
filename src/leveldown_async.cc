/* Copyright (c) 2012-2016 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

#include <leveldb/db.h>

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
  delete callback;
  callback = NULL;
}

void DestroyWorker::Execute () {
  leveldb::Options options;
  SetStatus(leveldb::DestroyDB(**location, options));
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
  delete callback;
  callback = NULL;
}

void RepairWorker::Execute () {
  leveldb::Options options;
  SetStatus(leveldb::RepairDB(**location, options));
}

} // namespace leveldown

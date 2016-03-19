/* Copyright (c) 2012-2016 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */


#include <leveldb/write_batch.h>
#include "batch.h"
#include "batch_async.h"

namespace leveldown {

/** NEXT WORKER **/

BatchWriteWorker::BatchWriteWorker (
    Batch* batch
  , Nan::Callback *callback
) : AsyncWorker(NULL, callback)
  , batch(batch)
{};

BatchWriteWorker::~BatchWriteWorker () {
  delete callback;
  callback = NULL;
}

void BatchWriteWorker::Execute () {
  SetStatus(batch->Write());
}

} // namespace leveldown

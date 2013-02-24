/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include "leveldb/write_batch.h"

#include "batch.h"

BEGIN_NAMESPACE(leveldown)

BatchOp::~BatchOp () {}

BatchDelete::~BatchDelete () {
  keyPtr.Dispose();
}

void BatchDelete::Execute (leveldb::WriteBatch* batch) {
  batch->Delete(key);
}

BatchWrite::~BatchWrite () {
  keyPtr.Dispose();
  valuePtr.Dispose();
}

void BatchWrite::Execute (leveldb::WriteBatch* batch) {
  batch->Put(key, value);
}

END_NAMESPACE(leveldown)

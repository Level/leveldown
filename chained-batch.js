const util = require('util')
const AbstractChainedBatch = require('abstract-leveldown').AbstractChainedBatch
const binding = require('./binding')

function ChainedBatch (db) {
  AbstractChainedBatch.call(this, db)
  this.context = binding.batch_init(db.context)
}

ChainedBatch.prototype._put = function (key, value) {
  binding.batch_put(this.context, key, value)
}

ChainedBatch.prototype._del = function (key) {
  binding.batch_del(this.context, key)
}

ChainedBatch.prototype._clear = function () {
  binding.batch_clear(this.context)
}

ChainedBatch.prototype._write = function (options, callback) {
  // TODO (ensure docs covers the following)
  // Note that we're passing in options here, which we didn't do before. We
  // must do this so we can use the `sync` property, which we didn't handle before
  // since we never passed in an object at time of creation (bug) (the previous c++
  // used to assume we did this from the js side from the ChainedBatch()
  // constructor).
  binding.batch_write(this.context, options, callback)
}

util.inherits(ChainedBatch, AbstractChainedBatch)

module.exports = ChainedBatch

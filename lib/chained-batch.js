
var util = require('util')

  , AbstractChainedBatch = require('abstract-leveldown').AbstractChainedBatch

  , ChainedBatch = function(db) {
      AbstractChainedBatch.call(this, db)

      this._binding = db._binding .batch()
    }

  , _put = function(key, value) {
      this._binding.put(key, value)
      return this
    }

  , _del = function(key) {
      this._binding.del(key)
      return this
    }

  , _clear = function() {
      this._binding.clear()
      return this
    }

  , _write = function(options, callback) {
      this._binding.write(options, callback)
    }

util.inherits(ChainedBatch, AbstractChainedBatch)

ChainedBatch.prototype._put = _put
ChainedBatch.prototype._del = _del
ChainedBatch.prototype._clear = _clear
ChainedBatch.prototype._write = _write

module.exports = ChainedBatch

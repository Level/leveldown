
var binding = require('bindings')('leveldown.node').leveldown

  , util = require('util')

  , AbstractLevelDOWN = require('abstract-leveldown').AbstractLevelDOWN

// constructor, passes through the 'location' argument to the AbstractLevelDOWN constructor
  , LevelDOWN = function (location) {
      AbstractLevelDOWN.call(this, location)

      this._binding = binding(location)

      // this._approximateSize = this._binding.approximateSize
    }

  , factory = function(location) {
      return new LevelDOWN(location)
    }

  , _approximateSize = function(start, end, callback) {
      this._binding.approximateSize(start, end, callback)
    }

  , _open = function(options, callback) {
      this._binding.open(options, callback)
    }

  , _close = function(callback) {
      this._binding.close(callback)
    }

  , _batch = function(array, options, callback) {
      this._binding.batch(array, options, callback)
    }

  , _get = function(key, options, callback) {
      this._binding.get(key, options, callback)
    }

  , _put = function(key, value, options, callback) {
      this._binding.put(key, value, options, callback)
    }

  , _del = function(key, options, callback) {
      this._binding.del(key, options, callback)
    }

  , _iterator = function(options) {
      return this._binding.iterator(options)
    }

  , getProperty = function(property) {
      return this._binding.getProperty(property)
    }

// our new prototype inherits from AbstractLevelDOWN
util.inherits(LevelDOWN, AbstractLevelDOWN)

LevelDOWN.prototype._open = _open
LevelDOWN.prototype._close = _close
LevelDOWN.prototype._approximateSize = _approximateSize
LevelDOWN.prototype._batch = _batch
LevelDOWN.prototype._get = _get
LevelDOWN.prototype._put = _put
LevelDOWN.prototype._del = _del
LevelDOWN.prototype._iterator = _iterator

LevelDOWN.prototype.getProperty = getProperty

factory.destroy = binding.destroy
factory.repair = binding.repair

module.exports = factory

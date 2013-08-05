
var binding = require('bindings')('leveldown.node').leveldown

  , util = require('util')

  , AbstractLevelDOWN = require('abstract-leveldown').AbstractLevelDOWN

  , ChainedBatch = require('./lib/chained-batch')

// constructor, passes through the 'location' argument to the AbstractLevelDOWN constructor
  , LevelDOWN = function (location) {
      if (!(this instanceof LevelDOWN))
        return new LevelDOWN(location)

      AbstractLevelDOWN.call(this, location)

      this._binding = binding(location)
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

  , _chainedBatch = function() {
      return new ChainedBatch(this)
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
      if (typeof(property) !== 'string')
        throw new Error('getProperty() requires a valid `property` argument')

      return this._binding.getProperty(property)
    }

  , destroy = function(location, callback) {
      if (arguments.length < 2)
        throw new Error("destroy() requires `location` and `callback` arguments");

      if (typeof(location) !== 'string')
        throw new Error("destroy() requires a location string argument");

      if (typeof(callback) !== 'function')
        throw new Error("destroy() requires a callback function argument");

      binding.destroy(location, callback)
    }


  , repair = function(location, callback) {
      if (arguments.length < 2)
        throw new Error("repair() requires `location` and `callback` arguments");

      if (typeof(location) !== 'string')
        throw new Error("repair() requires a location string argument");

      if (typeof(callback) !== 'function')
        throw new Error("repair() requires a callback function argument");

      binding.repair(location, callback)
    }

// our new prototype inherits from AbstractLevelDOWN
util.inherits(LevelDOWN, AbstractLevelDOWN)

LevelDOWN.prototype._open = _open
LevelDOWN.prototype._close = _close
LevelDOWN.prototype._approximateSize = _approximateSize
LevelDOWN.prototype._batch = _batch
LevelDOWN.prototype._chainedBatch = _chainedBatch
LevelDOWN.prototype._get = _get
LevelDOWN.prototype._put = _put
LevelDOWN.prototype._del = _del
LevelDOWN.prototype._iterator = _iterator

LevelDOWN.prototype.getProperty = getProperty

LevelDOWN.destroy = destroy
LevelDOWN.repair = repair

module.exports = LevelDOWN

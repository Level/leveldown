const util = require('util')
const AbstractLevelDOWN = require('abstract-leveldown').AbstractLevelDOWN
const binding = require('bindings')('leveldown').leveldown
const ChainedBatch = require('./chained-batch')
const Iterator = require('./iterator')

function LevelDOWN (location) {
  if (!(this instanceof LevelDOWN)) {
    return new LevelDOWN(location)
  }

  AbstractLevelDOWN.call(this, location)
  this.binding = binding(location)
}

util.inherits(LevelDOWN, AbstractLevelDOWN)

LevelDOWN.prototype._open = function (options, callback) {
  this.binding.open(options, callback)
}

LevelDOWN.prototype._close = function (callback) {
  this.binding.close(callback)
}

LevelDOWN.prototype._put = function (key, value, options, callback) {
  this.binding.put(key, value, options, callback)
}

LevelDOWN.prototype._get = function (key, options, callback) {
  this.binding.get(key, options, callback)
}

LevelDOWN.prototype._del = function (key, options, callback) {
  this.binding.del(key, options, callback)
}

LevelDOWN.prototype._chainedBatch = function () {
  return new ChainedBatch(this)
}

LevelDOWN.prototype._batch = function (operations, options, callback) {
  return this.binding.batch(operations, options, callback)
}

LevelDOWN.prototype.approximateSize = function (start, end, callback) {
  if (start == null ||
      end == null ||
      typeof start === 'function' ||
      typeof end === 'function') {
    throw new Error('approximateSize() requires valid `start`, `end` and `callback` arguments')
  }

  if (typeof callback !== 'function') {
    throw new Error('approximateSize() requires a callback argument')
  }

  start = this._serializeKey(start)
  end = this._serializeKey(end)

  this.binding.approximateSize(start, end, callback)
}

LevelDOWN.prototype.compactRange = function (start, end, callback) {
  this.binding.compactRange(start, end, callback)
}

LevelDOWN.prototype.getProperty = function (property) {
  if (typeof property !== 'string') {
    throw new Error('getProperty() requires a valid `property` argument')
  }

  return this.binding.getProperty(property)
}

LevelDOWN.prototype._iterator = function (options) {
  return new Iterator(this, options)
}

LevelDOWN.destroy = function (location, callback) {
  if (arguments.length < 2) {
    throw new Error('destroy() requires `location` and `callback` arguments')
  }
  if (typeof location !== 'string') {
    throw new Error('destroy() requires a location string argument')
  }
  if (typeof callback !== 'function') {
    throw new Error('destroy() requires a callback function argument')
  }

  binding.destroy(location, callback)
}

LevelDOWN.repair = function (location, callback) {
  if (arguments.length < 2) {
    throw new Error('repair() requires `location` and `callback` arguments')
  }
  if (typeof location !== 'string') {
    throw new Error('repair() requires a location string argument')
  }
  if (typeof callback !== 'function') {
    throw new Error('repair() requires a callback function argument')
  }

  binding.repair(location, callback)
}

module.exports = LevelDOWN.default = LevelDOWN

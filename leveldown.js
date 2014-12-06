const util              = require('util')
    , AbstractLevelDOWN = require('abstract-leveldown').AbstractLevelDOWN

    , binding           = require('bindings')('leveldown.node').leveldown

    , ChainedBatch      = require('./chained-batch')
    , Iterator          = require('./iterator')


function LevelDOWN (location) {
  if (!(this instanceof LevelDOWN))
    return new LevelDOWN(location)

  AbstractLevelDOWN.call(this, location)
  this.binding = binding(location)
}

util.inherits(LevelDOWN, AbstractLevelDOWN)

LevelDOWN.prototype._openSync = function (options) {
  return this.binding.openSync(options)
}

LevelDOWN.prototype._open = function (options, callback) {
  return this.binding.open(options, callback)
}

LevelDOWN.prototype._close = function (callback) {
  this.binding.close(callback)
}

LevelDOWN.prototype._getSync = function (key, options) {
  var fillCache = true;
  var asBuffer = false;
  if (typeof options === 'object') {
    if (options.fillCache === false) fillCache = false;
    if (options.asBuffer === true) asBuffer = true;
  }
  var result = this.binding.getSync(key, fillCache);
  if (asBuffer) result = new Buffer(result);
  return result;
}

LevelDOWN.prototype._putSync = function (key, value, options) {
  var flushSync = false;
  if (typeof options === 'object' && options.sync === true) flushSync = true;
  return this.binding.putSync(key, value, flushSync)
}

LevelDOWN.prototype._delSync = function (key, options) {
  var flushSync = false;
  if (typeof options === 'object' && options.sync === true) flushSync = true;
  return this.binding.delSync(key, flushSync)
}

LevelDOWN.prototype._batchSync = function (operations, options) {
  var flushSync = false;
  if (typeof options === 'object' && options.sync === true) flushSync = true;
  return this.binding.batchSync(operations, flushSync)
}

LevelDOWN.prototype._approximateSizeSync = function (start, end) {
  return this.binding.approximateSizeSync(start, end)
}


LevelDOWN.prototype._put = function (key, value, options, callback) {
  return this.binding.put(key, value, options, callback)
}

LevelDOWN.prototype._get = function (key, options, callback) {
  return this.binding.get(key, options, callback)
}

LevelDOWN.prototype._del = function (key, options, callback) {
  return this.binding.del(key, options, callback)
}


LevelDOWN.prototype._chainedBatch = function () {
  return new ChainedBatch(this)
}


LevelDOWN.prototype._batch = function (operations, options, callback) {
  return this.binding.batch(operations, options, callback)
}


LevelDOWN.prototype._approximateSize = function (start, end, callback) {
  return this.binding.approximateSize(start, end, callback)
}


LevelDOWN.prototype.getProperty = function (property) {
  if (typeof property != 'string')
    throw new Error('getProperty() requires a valid `property` argument')

  return this.binding.getProperty(property)
}


LevelDOWN.prototype._iterator = function (options) {
  return new Iterator(this, options)
}


LevelDOWN.destroy = function (location, callback) {
  if (arguments.length < 2)
    throw new Error('destroy() requires `location` and `callback` arguments')

  if (typeof location != 'string')
    throw new Error('destroy() requires a location string argument')

  if (typeof callback != 'function')
    throw new Error('destroy() requires a callback function argument')

  binding.destroy(location, callback)
}


LevelDOWN.repair = function (location, callback) {
  if (arguments.length < 2)
    throw new Error('repair() requires `location` and `callback` arguments')

  if (typeof location != 'string')
    throw new Error('repair() requires a location string argument')

  if (typeof callback != 'function')
    throw new Error('repair() requires a callback function argument')

  binding.repair(location, callback)
}


module.exports = LevelDOWN

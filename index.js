var util = require('util')
  , AbstractLevelDOWN = require('abstract-leveldown').AbstractLevelDOWN

  , binding = require('bindings')('leveldown.node').leveldown

  , Iterator = require('./lib/iterator')

  , LevelDOWN = function (location) {
      if (!(this instanceof LevelDOWN))
        return new LevelDOWN(location)

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

LevelDOWN.prototype._batch = function (operations, options, callback) {
  if (arguments.length === 0)
    return this.binding.batch()

  return this.binding.batch(operations, options, callback)
}

LevelDOWN.prototype._approximateSize = function (start, end, callback) {
  this.binding.approximateSize(start, end, callback)
}

LevelDOWN.prototype.getProperty = function (property) {
  return this.binding.getProperty(property)
}

LevelDOWN.prototype._iterator = function (options) {
  return new Iterator(this.binding, options)
}

LevelDOWN.destroy = binding.destroy
LevelDOWN.repair = binding.repair

module.exports = LevelDOWN

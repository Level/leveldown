var binding = require('bindings')('leveldown.node').leveldown

  , Iterator = require('./lib/iterator')

  , LevelDown = function (options) {
      if (!(this instanceof LevelDown))
        return new LevelDown(options)

      this.binding = binding(options)
    }

LevelDown.prototype.open = function (options, callback) {
  this.binding.open(options, callback)
}

LevelDown.prototype.close = function (callback) {
  this.binding.close(callback)
}

LevelDown.prototype.batch = function (operations, options, callback) {
  if (arguments.length === 0)
    return this.binding.batch()

  return this.binding.batch(operations, options, callback)
}

LevelDown.prototype.get = function (key, options, callback) {
  this.binding.get(key, options, callback)
}

LevelDown.prototype.getProperty = function (property) {
  return this.binding.getProperty(property)
}

LevelDown.prototype.del = function (key, options, callback) {
  this.binding.del(key, options, callback)
}

LevelDown.prototype.put = function (key, value, options, callback) {
  this.binding.put(key, value, options, callback)
}

LevelDown.prototype.approximateSize = function (start, end, callback) {
  this.binding.approximateSize(start, end, callback)
}

LevelDown.prototype.iterator = function (options) {
  return new Iterator(this.binding, options)
}

LevelDown.destroy = binding.destroy
LevelDown.repair = binding.repair

module.exports = LevelDown
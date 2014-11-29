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

//the optimal low-level sync functions:
AbstractLevelDOWN.prototype.getSync = function (key, options) {
  if (this._getSync) {
    var result = this._getSync(key, options)
    return result
  }
}

AbstractLevelDOWN.prototype.putSync = function (key, value, options) {
  if (this._putSync) {
    var result = this._putSync(key, value, options)
    return result
  }
}

AbstractLevelDOWN.prototype.delSync = function (key, flushSync) {
  if (this._delSync) {
    var result = this._delSync(key, options)
    return result
  }
}

AbstractLevelDOWN.prototype.batchSync = function (operations, options) {
  if (this._batchSync) {
    var result = this._batchSync(operations, options)
    return result
  }
}

AbstractLevelDOWN.prototype.openSync = function (options) {
  if (this._openSync) {
    var result = this._openSync(options)
    return result
  }
}

AbstractLevelDOWN.prototype.open = function (options, callback) {
  if (typeof options == 'function')
    callback = options


  if (typeof options != 'object')
    options = {}

  options.createIfMissing = options.createIfMissing != false
  options.errorIfExists = !!options.errorIfExists

  if (typeof this._open == 'function')
    return this._open(options, callback)

  if (callback) process.nextTick(callback)
}

AbstractLevelDOWN.prototype.get = function (key, options, callback) {
  var err

  if (typeof options == 'function')
    callback = options

  if (err = this._checkKey(key, 'key', this._isBuffer)) {
    if (callback)
      return callback(err)
    else
      throw err
  }

  if (!this._isBuffer(key))
    key = String(key)

  if (typeof options != 'object')
    options = {}

  options.asBuffer = options.asBuffer != false

  if (callback) {
    if (typeof this._get === 'function')
      return this._get(key, options, callback)
    else if (typeof this._getSync === 'function') 
      process.nextTick(function () {
        try {
          callback(null, this._getSync(key, options))
        } catch (err) {
          callback(err)
        }
      })
    process.nextTick(function () { callback(new Error('NotFound')) })
  } else {
    if (typeof this._getSync === 'function') return this._getSync(key, options)
    throw new Error('NotFound')
  }
}

AbstractLevelDOWN.prototype.put = function (key, value, options, callback) {
  var err

  if (typeof options == 'function')
    callback = options

  if (err = this._checkKey(key, 'key', this._isBuffer)) {
    if (callback)
      return callback(err)
    else
      throw err
  }

  if (!this._isBuffer(key))
    key = String(key)

  // coerce value to string in node, don't touch it in browser
  // (indexeddb can store any JS type)
  if (value != null && !this._isBuffer(value) && !process.browser)
    value = String(value)

  if (typeof options != 'object')
    options = {}

  if (callback) {
    if (typeof this._put == 'function')
      return this._put(key, value, options, callback)
    else if (typeof this._putSync === 'function') 
      process.nextTick(function () {
        try {
          callback(null, this._putSync(key, value, options))
        } catch (err) {
          callback(err)
        }
      })
    process.nextTick(callback)
  } else {
    if (typeof this._putSync === 'function')
      return this._putSync(key, value, options)
  }
}

AbstractLevelDOWN.prototype.del = function (key, options, callback) {
  var err

  if (typeof options == 'function')
    callback = options

  if (err = this._checkKey(key, 'key', this._isBuffer)) {
    if (callback)
      return callback(err)
    else
      throw err
  }

  if (!this._isBuffer(key))
    key = String(key)

  if (typeof options != 'object')
    options = {}

  if (callback) {
    if (typeof this._del == 'function')
      return this._del(key, options, callback)
    else if (typeof this._delSync === 'function') 
      process.nextTick(function () {
        try {
          callback(null, this._delSync(key, options))
        } catch (err) {
          callback(err)
        }
      })

    process.nextTick(callback)
  }
  else {
    if (typeof this._delSync == 'function')
      return this._delSync(key, options)
  }
}

AbstractLevelDOWN.prototype.batch = function (array, options, callback) {
  if (!arguments.length)
    return this._chainedBatch()

  if (typeof options == 'function')
    callback = options

  if (typeof array == 'function')
    callback = array

  if (!Array.isArray(array)) {
    var vError = new Error('batch(array) requires an array argument')
    if (callback)
      return callback(vError)
    else
      throw vError
  }

  if (!options || typeof options != 'object')
    options = {}

  var i = 0
    , l = array.length
    , e
    , err

  for (; i < l; i++) {
    e = array[i]
    if (typeof e != 'object')
      continue

    if (err = this._checkKey(e.type, 'type', this._isBuffer)) {
      if (callback)
        return callback(err)
      else
        throw err
    }

    if (err = this._checkKey(e.key, 'key', this._isBuffer))
      if (callback)
        return callback(err)
      else
        throw err
  }

  if (callback) {
    if (typeof this._batch == 'function')
      return this._batch(array, options, callback)
    else if (typeof this._batchSync === 'function') 
      process.nextTick(function () {
        try {
          callback(null, this._batchSync(array, options))
        } catch (err) {
          callback(err)
        }
      })

    process.nextTick(callback)
  } else {
    if (typeof this._batchSync == 'function')
      return this._batchSync(array, options)
  }
}

AbstractLevelDOWN.prototype.approximateSize = function (start, end, callback) {
  if (   start == null
      || end == null
      || typeof start == 'function'
      || typeof end == 'function') {
    throw new Error('approximateSize() requires valid `start`, `end` arguments')
  }

  if (!this._isBuffer(start))
    start = String(start)

  if (!this._isBuffer(end))
    end = String(end)

  if (typeof this._approximateSize == 'function')
    return this._approximateSize(start, end, callback)
  if (callback)
    process.nextTick(function () {
      callback(null, 0)
    })
}


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

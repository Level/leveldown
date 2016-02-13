const util             = require('util')
    , AbstractIterator = require('abstract-leveldown').AbstractIterator
    , fastFuture       = require('fast-future')
    , compare          = require('ltgt').compare


function Iterator (db, options) {
  AbstractIterator.call(this, db)

  this.reverse    = options && options.reverse
  this.binding    = db.binding.iterator(options)
  this.cache      = null
  this.finished   = false
  this.fastFuture = fastFuture()
}

util.inherits(Iterator, AbstractIterator)

Iterator.prototype.seek = function (target) {
  if (this._ended)
    throw new Error('cannot call seek() after end()')
  if (this._nexting)
    throw new Error('cannot call seek() before next() has completed')
  if (typeof target !== 'string' && !Buffer.isBuffer(target))
    throw new Error('seek() requires a string or buffer target')

  // Check if target is cached
  var length = this.cache ? this.cache.length : 0

  if (length) {
    if (typeof target === 'string') {
      target = new Buffer(target)
    }

    for(var i=length-1; i>0; i-=2) {
      var c = compare(this.cache[i], target)
      if (this.reverse ? c <= 0 : c >= 0) {
        return void this.cache.splice(i+1, length)
      }
    }
  }

  this.cache = null
  this.binding.seek(target)
}

Iterator.prototype._next = function (callback) {
  var that = this
    , key
    , value

  if (this.cache && this.cache.length) {
    key   = this.cache.pop()
    value = this.cache.pop()

    this.fastFuture(function () {
      callback(null, key, value)
    })

  } else if (this.finished) {
    this.fastFuture(function () {
      callback()
    })
  } else {
    this.binding.next(function (err, array, finished) {
      if (err) return callback(err)

      that.cache    = array
      that.finished = finished
      that._next(callback)
    })
  }

  return this
}


Iterator.prototype._end = function (callback) {
  delete this.cache
  this.binding.end(callback)
}


module.exports = Iterator

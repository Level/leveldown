const util             = require('util')
    , AbstractIterator = require('abstract-leveldown').AbstractIterator
    , fastFuture       = require('fast-future')


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

  this.finished = false
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

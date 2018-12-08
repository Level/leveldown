const util = require('util')
const AbstractIterator = require('abstract-leveldown').AbstractIterator
const fastFuture = require('fast-future')
const binding = require('./binding')

function Iterator (db, options) {
  AbstractIterator.call(this, db)

  this.context = binding.iterator_init(db.context, options)
  this.cache = null
  this.finished = false
  this.fastFuture = fastFuture()
}

util.inherits(Iterator, AbstractIterator)

Iterator.prototype._seek = function (target) {
  if (target.length === 0) {
    throw new Error('cannot seek() to an empty target')
  }

  this.cache = null
  binding.iterator_seek(this.context, target)
  this.finished = false
}

Iterator.prototype._next = function (callback) {
  var that = this
  var key
  var value

  if (this.cache && this.cache.length) {
    key = this.cache.pop()
    value = this.cache.pop()

    this.fastFuture(function () {
      callback(null, key, value)
    })
  } else if (this.finished) {
    this.fastFuture(function () {
      callback()
    })
  } else {
    binding.iterator_next(this.context, function (err, array, finished) {
      if (err) return callback(err)

      that.cache = array
      that.finished = finished
      that._next(callback)
    })
  }

  return this
}

Iterator.prototype._end = function (callback) {
  delete this.cache
  binding.iterator_end(this.context, callback)
}

module.exports = Iterator

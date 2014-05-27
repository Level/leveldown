var Iterator = function (binding, options) {
      this.binding = binding.iterator(options)
      this.cache = null
      this.finished = false
      this.fastFuture = require('fast-future')()
    }

Iterator.prototype.next = function (callback) {
  var self = this
    , key
    , value

  if (!callback)
    throw new Error('next() requires a callback argument')

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
    self.binding.next(function (err, array, finished) {
      if (err) return callback(err)

      self.cache = array
      self.finished = finished
      self.next(callback)
    })
  }
  return this
}

Iterator.prototype.end = function (callback) {
  delete this.cache
  this.binding.end(callback)
}

module.exports = Iterator
var Iterator = function (binding, options) {
      this.binding = binding.iterator(options)
      this.cache = null
      this.fastFuture = require('fast-future')()
    }

Iterator.prototype.next = function (callback) {
  var self = this
    , row

  if (!callback)
    throw new Error('next() requires a callback argument')

  if (this.cache && this.cache.length) {
    row = this.cache.shift()

    this.fastFuture(function () {
      if (row === null)
        callback()
      else
        callback(null, row.key, row.value)
    })

  } else {
    self.binding.next(function (err, array) {
      if (err) return callback(err)

      self.cache = array
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
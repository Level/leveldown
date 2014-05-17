var nextTick = global.setImmediate || process.nextTick

  , Iterator = function (binding, options) {
      this.binding = binding.iterator(options)
      this.cache = null
    }

Iterator.prototype.next = function (callback) {
  var that = this
    , row

  if (!callback)
    throw new Error('next() requires a callback argument')

  if (this.cache && this.cache.length) {
    row = this.cache.shift()

    nextTick(function () {
      if (row === null)
        callback()
      else
        callback(null, row.key, row.value)
      
    })

  } else {
    that.binding.next(function (err, array) {
      if (err) return callback(err)

      that.cache = array
      that.next(callback)
    })
  }
  return this
}

Iterator.prototype.end = function (callback) {
  delete this.cache
  this.binding.end(callback)
}

module.exports = Iterator
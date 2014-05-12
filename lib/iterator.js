var util = require('util')
  , AbstractIterator = require('abstract-leveldown').AbstractIterator

  , Iterator = function (db, options) {
      AbstractIterator.call(this, options)
      this.binding = db.binding.iterator(options)
      this.cache = null
    }

util.inherits(Iterator, AbstractIterator)

Iterator.prototype._next = function (callback) {
  var that = this
    , row

  if (this.cache && this.cache.length) {
    row = this.cache.shift()

    process.nextTick(function () {
      if (row === null)
        callback()
      else
        callback(null, row.key, row.value)
      
    })

  } else {
    that.binding.next(function (err, array) {
      if (err) return callback(err)

      that.cache = array

      var row = array.shift()

      if (row === null)
        callback()
      else
        callback(null, row.key, row.value)
    })
  }
  return this
}

Iterator.prototype._end = function (callback) {
  delete this.cache
  this.binding.end(callback)
}

module.exports = Iterator
var Iterator = function (binding, options) {

      this.binding = binding._iterator(options)
      this.buffer = []
    }

Iterator.prototype.next = function (callback) {
  var that = this

  if (!callback)
    throw new Error('next() requires a callback argument')

  if (this.buffer && this.buffer.length) {
    var row = this.buffer.shift()

    if (row === undefined)
      callback()
    else
      callback(null, row.key, row.value)

  } else {
    that.binding.next(function (err, array) {
      if (err) return callback(err)

      that.buffer = array
      that.next(callback)
    })
  }
  return this
}

Iterator.prototype.end = function (callback) {
  delete this.buffer
  this.binding.end(callback)
}

module.exports = function (binding) {
  return function (options) {
    return new Iterator(binding, options)
  }
}
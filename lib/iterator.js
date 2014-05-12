var Iterator = function (binding, options) {

      this.binding = binding._iterator(options)
      this.buffer = []
    }

Iterator.prototype.next = function (callback) {
  if (!callback)
    throw new Error('next() requires a callback argument')

  this.binding.next(function (err, key, value) {
    if (!arguments.length)
      callback()
    else
      callback(err, key, value)
  })
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
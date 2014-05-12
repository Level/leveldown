var Iterator = function (binding, options) {

      this.binding = binding._iterator(options)
    }

Iterator.prototype.next = function (callback) {
  this.binding.next(callback)
  return this
}

Iterator.prototype.end = function (callback) {
  this.binding.end(callback)
}

module.exports = function (binding) {
  return function (options) {
    return new Iterator(binding, options)
  }
}
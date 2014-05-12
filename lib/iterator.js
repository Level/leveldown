var Iterator = function (binding, options) {

      this.binding = binding._iterator(options)
      this.buffer = []
    }

Iterator.prototype._getBuffer = function (callback, buffer) {
  var that = this

  buffer = buffer || []

  if (buffer.length === 3) {
    callback(null, buffer)
  } else {
    this.binding.next(function (err, key, value) {
      if (err)
        callback(err)
      else if (!arguments.length) {
        buffer.push(null)
        callback(null, buffer)
      } else {
        buffer.push([ key, value ])
        that._getBuffer(callback, buffer)
      }
    })
  }
}

Iterator.prototype.next = function (callback) {
  var that = this

  if (!callback)
    throw new Error('next() requires a callback argument')

  if (this.buffer && this.buffer.length) {
    var row = this.buffer.shift();

    if (row === null)
      callback()
    else
      callback(null, row[0], row[1])
  } else {
  
    this._getBuffer(function (err, array) {
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
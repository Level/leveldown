const crypto = require('crypto')

function RandomData (bufSize) {
  if (!(this instanceof RandomData))
    return new RandomData(bufSize)

  if (!bufSize)
    bufSize = 64 << 10 // 64KB

  this._buffer = crypto.randomBytes(bufSize)
  this._pos    = 0
}

RandomData.prototype.generate = function (length) {
  if (this._pos + length > this._buffer.length)
    this._pos = 0

  this._pos += length
  return this._buffer.slice(this._pos - length, this._pos)
}

module.exports = RandomData
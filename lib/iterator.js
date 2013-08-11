
var util = require('util')

  , AbstractIterator = require('abstract-leveldown').AbstractIterator

  , Iterator = function(db, _options) {
      AbstractIterator.call(this, db)

      this._options = _options
      this._binding = db._binding.iterator(_options)
    }

  , _next = function(callback) {
      this._binding.next(callback)
      return this
    }

  , _end = function(callback) {
      this._binding.end(callback)
    }

util.inherits(Iterator, AbstractIterator)

Iterator.prototype._next = _next
Iterator.prototype._end = _end

module.exports = Iterator

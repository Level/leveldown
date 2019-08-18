const test = require('tape')
const tempy = require('tempy')
const leveldown = require('..')
const suite = require('abstract-leveldown/test')

module.exports = suite.common({
  test: test,
  factory: function () {
    return leveldown(tempy.directory())
  },
  clear: true
})

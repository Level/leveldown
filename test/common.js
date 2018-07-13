const test = require('tape')
const tempy = require('tempy')
const leveldown = require('..')

module.exports = require('abstract-leveldown/test/common')({
  test: test,
  factory: function () {
    return leveldown(tempy.directory())
  }
})

const test = require('tape')
const testCommon = require('abstract-leveldown/test/common')
const leveldown = require('..')
const abstract = require('abstract-leveldown/test/close-test')

module.exports.setUp = function () {
  test('setUp', testCommon.setUp)
}

module.exports.close = abstract.close

module.exports.tearDown = function () {
  test('tearDown', testCommon.tearDown)
}

module.exports.all = function (leveldown) {
  module.exports.setUp()
  module.exports.close(leveldown, test, testCommon)
  module.exports.tearDown()
}

module.exports.all(leveldown)

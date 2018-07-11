const testCommon = require('abstract-leveldown/test/common')
const tempy = require('tempy')
const leveldown = require('..')

testCommon.factory = function () {
  return leveldown(tempy.directory())
}

module.exports = testCommon

const test = require('tape')
const testCommon = require('./common')
const abstract = require('abstract-leveldown/test/leveldown-test')
const leveldown = require('..')

abstract.args(testCommon.factory, test)

test('test database creation non-string location throws', function (t) {
  t.throws(
    leveldown.bind(null, {}),
    /constructor requires a location string argument/,
    'non-string location leveldown() throws'
  )
  t.end()
})

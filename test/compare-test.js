const test       = require('tap').test
, testCommon = require('abstract-leveldown/testCommon')
, leveldown  = require('../')

var db

test('setUp common', testCommon.setUp)

test('setUp db', function (t) {
  db = leveldown(testCommon.location())
  db.open(t.end.bind(t))
})

test('test argument-less compare() throws', function (t) {
  t.throws(
    db.compare.bind(db)
    , { name: 'Error', message: 'compare() requires valid `keyA`, `keyB` arguments' }
    , 'no-arg compare() throws'
  )
  t.end()
})

test('test compare() returns integer', function (t) {
  t.equal(db.compare('foo', 'foo'), 0, 'keys are equal')
  t.equal(db.compare('foo', 'bar'), 1, 'keys are not equal')
  t.end()
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})

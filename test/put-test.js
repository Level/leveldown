const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

test('setUp', function (t) {
  db = leveldown(testCommon.location())
  db.open(testCommon.setUp.bind(null, t))
})

test('test argument-less put() throws', function (t) {
  t.throws(db.put.bind(db), 'no-arg put() throws')
  t.end()
})

test('test callback-less, 1-arg, put() throws', function (t) {
  t.throws(db.put.bind(db, 'foo'), 'callback-less, 1-arg put() throws')
  t.end()
})

test('test callback-less, 2-arg, put() throws', function (t) {
  t.throws(db.put.bind(db, 'foo', 'bar'), 'callback-less, 2-arg put() throws')
  t.end()
})

test('test callback-less, 3-arg, put() throws', function (t) {
  t.throws(db.put.bind(db, 'foo', 'bar', {}), 'callback-less, 3-arg put() throws')
  t.end()
})

test('test simple put()', function (t) {
  db.put('foo', 'bar', function (err) {
    t.notOk(err, 'no error')
    db.get('foo', function (err, value) {
      t.notOk(err, 'no error')
      t.equal(value.toString(), 'bar')
      t.end()
    })
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})
const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

test('setUp', function (t) {
  db = leveldown(testCommon.location())
  db.open(testCommon.setUp.bind(null, t))
})

test('test argument-less batch() throws', function (t) {
  t.throws(db.batch.bind(db), 'no-arg batch() throws')
  t.end()
})

test('test callback-less, 1-arg, batch() throws', function (t) {
  t.throws(db.batch.bind(db, []), 'callback-less, 1-arg batch() throws')
  t.end()
})

test('test callback-less, 3-arg, batch() throws', function (t) {
  t.throws(db.batch.bind(db, [], {}), 'callback-less, 3-arg batch() throws')
  t.end()
})

test('test batch() with wrong arg throws', function (t) {
  t.throws(db.batch.bind(db, {}, {}), 'wrong arg type throws')
  t.end()
})

test('test batch() with empty array', function (t) {
  db.batch([], function (err) {
    t.notOk(err, 'no error')
    t.end()
  })
})

test('test simple batch()', function (t) {
  db.batch([{ type: 'put', key: 'foo', value: 'bar' }], function (err) {
    t.notOk(err, 'no error')

    db.get('foo', function (err, value) {
      t.notOk(err, 'no error')
      t.type(value, Buffer)
      t.equal(value.toString(), 'bar')

      t.end()
    })
  })
})

test('test batch() with missing `value`', function (t) {
  db.batch([{ type: 'put', key: 'foo1' }], function (err) {
    t.notOk(err, 'no error')

    db.get('foo1', function (err, value) {
      t.ok(err, 'entry not found')
      t.notOk(value, 'value not returned')
      t.like(err.message, /NotFound/)
      t.end()
    })
  })
})

test('test batch() with missing `key`', function (t) {
  db.batch([{ type: 'put', value: 'foo1' }], function (err) {
    t.notOk(err, 'no error')
    t.end()
  })
})

test('test batch() with missing `key` and `value`', function (t) {
  db.batch([{ type: 'put' }], function (err) {
    t.notOk(err, 'no error')
    t.end()
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})
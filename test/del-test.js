const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

test('setUp', function (t) {
  db = leveldown(testCommon.location())
  db.open(testCommon.setUp.bind(null, t))
})

test('test argument-less del() throws', function (t) {
  t.throws(db.del.bind(db), 'no-arg del() throws')
  t.end()
})

test('test callback-less, 1-arg, del() throws', function (t) {
  t.throws(db.del.bind(db, 'foo'), 'callback-less, 1-arg del() throws')
  t.end()
})

test('test callback-less, 3-arg, del() throws', function (t) {
  t.throws(db.del.bind(db, 'foo', {}), 'callback-less, 2-arg del() throws')
  t.end()
})

test('test simple del()', function (t) {
  db.put('foo', 'bar', function (err) {
    t.notOk(err, 'no error')
    db.del('foo', function (err) {
      t.notOk(err, 'no error')
      db.get('foo', function (err) {
        t.ok(err, 'entry propertly deleted')
        t.like(err.message, /NotFound/)
        t.end()
      })

    })
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})
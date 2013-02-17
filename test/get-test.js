const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

test('setUp', function (t) {
  db = leveldown(testCommon.location())
  db.open(testCommon.setUp.bind(null, t))
})

test('test argument-less get() throws', function (t) {
  t.throws(db.get.bind(db), 'no-arg get() throws')
  t.end()
})

test('test callback-less, 1-arg, get() throws', function (t) {
  t.throws(db.get.bind(db, 'foo'), 'callback-less, 1-arg get() throws')
  t.end()
})

test('test callback-less, 3-arg, get() throws', function (t) {
  t.throws(db.get.bind(db, 'foo', {}), 'callback-less, 2-arg get() throws')
  t.end()
})

test('test simple get()', function (t) {
  db.put('foo', 'bar', function (err) {
    t.notOk(err, 'no error')
    db.get('foo', function (err, value) {
      t.notOk(err, 'no error')
      t.type(value, Buffer)
      t.equal(value.toString(), 'bar')

      db.get('foo', {}, function (err, value) { // same but with {}
        t.notOk(err, 'no error')
        t.type(value, Buffer)
        t.equal(value.toString(), 'bar')

        db.get('foo', { asBuffer: false }, function (err, value) {
          t.notOk(err, 'no error')
          t.type(value, 'string')
          t.equal(value, 'bar')
          t.end()
        })
      })
    })
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})
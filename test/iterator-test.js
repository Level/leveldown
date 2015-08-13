const test       = require('tape')
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , abstract   = require('abstract-leveldown/abstract/iterator-test')
    , make       = require('./make')

abstract.all(leveldown, test, testCommon)

make('iterator throws if key is not a string', function (db, t, done) {
  var ite = db.iterator()
  var error
  try {
    ite.seek()
  } catch (e) {
    error = e
  }

  t.ok(error, 'had error')
  t.end()
})

make('iterator is seekable', function (db, t, done) {
  var ite = db.iterator()
  ite.seek('two')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key.toString(), 'two', 'key matches')
    t.same(value.toString(), '2', 'value matches')
    ite.next(function (err, key, value) {
      t.error(err, 'no error')
      t.same(key, undefined, 'end of iterator')
      t.same(value, undefined, 'end of iterator')
      ite.end(done)
    })
  })
})

make('reverse seek in the middle', function (db, t, done) {
  var ite = db.iterator({reverse: true, limit: 1})
  ite.seek('three!')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key.toString(), 'three', 'key matches')
    t.same(value.toString(), '3', 'value matches')
    ite.end(done)
  })
})

make('iterator invalid seek', function (db, t, done) {
  var ite = db.iterator()
  ite.seek('zzz')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key, undefined, 'end of iterator')
    t.same(value, undefined, 'end of iterator')
    ite.end(done)
  })
})

make('reverse seek from invalid range', function (db, t, done) {
  var ite = db.iterator({reverse: true})
  ite.seek('zzz')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key.toString(), 'two', 'end of iterator')
    t.same(value.toString(), '2', 'end of iterator')
    ite.end(done)
  })
})

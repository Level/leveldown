const fs         = require('fs')
    , test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

test('setUp', testCommon.setUp)

test('test database open no-arg throws', function (t) {
  t.throws(leveldown(testCommon.location()).open, 'no-arg open() throws')
  t.end()
})

test('test callback-less, 1-arg, open() throws', function (t) {
  var db = leveldown(testCommon.location())
  t.throws(db.open.bind(db, {}), 'callback-less, 1-arg open() throws')
  t.end()
})

test('test database open', function (t) {
  var db = leveldown(testCommon.location())

  // default createIfMissing=true, errorIfExists=false
  db.open(function (err) {
      t.notOk(err, 'no error')
      db.close(function () {
        t.end()
      })
    })
})

test('test database open createIfMissing:false', function (t) {
  var db = leveldown(testCommon.location())

  db.open({ createIfMissing: false }, function (err) {
    t.ok(err, 'error')
    t.ok(/does not exist/.test(err.message), 'error is about dir not existing')
    t.end()
  })
})

test('test database open errorIfExists:true', function (t) {
  var location = testCommon.location()
    , db       = leveldown(location)

  // make a valid database first, then close and dispose
  db.open(function (err) {
    t.notOk(err, 'no error')
    db.close(function (err) {
      t.notOk(err, 'no error')

      // open again with 'errorIfExists'
      db = leveldown(location)
      db.open({ createIfMissing: false, errorIfExists: true }, function (err) {
        t.ok(err, 'error')
        t.ok(/exists/.test(err.message), 'error is about already existing')
        t.end()
      })
    })
  })
})

test('tearDown', testCommon.tearDown)
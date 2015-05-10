const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , fs         = require('fs')
    , join       = require('path').join

var db
  , location

test('setUp common', testCommon.setUp)

test('setUp db', function (t) {
  location = testCommon.location()
  db = leveldown(location)
  db.open(function (err) {
    t.ok(!err, 'no error')
    var key = 'beep'
      , value = 'boop'
    db.put(key, value, function (err) {
      t.ok(!err, 'no put error')
      db.get(key, function (err, _value) {
        t.equal(_value.toString(), value)
        t.end()
      })
    })
  })
})

test('liveBackup', function (t) {
  var expected = {
    name: 'Error',
    message: 'liveBackup() requires a valid `name` argument'
  }
  t.throws(db.liveBackup.bind(db, null), expected)
  t.throws(db.liveBackup.bind(db), expected)
  t.throws(db.liveBackup.bind(db, function () {}), expected)
  var now = Date.now()
  db.liveBackup(now, function (err) {
    t.ok(!err, 'no liveBackup error')
    var backup = join(location, 'backup-' + now)
    fs.stat(backup, function (err, stats) {
      t.ok(!err, 'no error')
      t.ok(stats.isDirectory(), 'backup dir should exist')
      fs.readdir(backup, function (err, files) {
        t.ok(!err, 'no error')
        t.ok(files.length > 0, 'should contain some files')
        t.end()
      })
    })
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})

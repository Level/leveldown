const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')

var db

test('setUp common', testCommon.setUp)

test('setUp db', function (t) {
  db = leveldown(testCommon.location())
  db.open(t.end.bind(t))
})

test('liveBackup', function (t) {
  var key = 'beep'
    , value = 'boop'
  db.put(key, value, function (err) {
    t.ok(!err, 'no put error')
    db.get(key, function (err, _value) {
      t.equal(_value.toString(), value)
      var backup = 'backup-' + Date.now()
      db.liveBackup(backup, function (err) {
        t.ok(!err, 'no liveBackup error')
        t.end()
      })
    })
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})

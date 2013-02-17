const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

test('setUp', testCommon.setUp)

test('test database close', function (t) {
  var db = leveldown(testCommon.location())

  db.open(function (err) {
    t.notOk(err, 'no error')
    t.throws(db.close.bind(db), 'no-arg close() throws')

    db.close(function (err) {
      t.notOk(err, 'no error')
      t.end()
    })
  })
})

test('tearDown', testCommon.tearDown)
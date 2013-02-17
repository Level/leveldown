const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

test('setUp', testCommon.setUp)

test('test close()', function (t) {
  var db = leveldown(testCommon.location())

  db.open(function (err) {
    t.notOk(err, 'no error')
    t.throws(db.close.bind(db), 'no-arg close() throws')
    t.throws(db.close.bind(db, 'foo'), 'non-callback close() throws')

    db.close(function (err) {
      t.notOk(err, 'no error')
      t.end()
    })
  })
})

test('tearDown', testCommon.tearDown)
const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

test('setUp', function (t) {
  db = leveldown(testCommon.location())
  db.open(testCommon.setUp.bind(null, t))
})

test('writes', function (t) {
  var batch = new leveldown.CBatch()
  batch.put('foo', 'bar')
  db.write(batch, function (err) {
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

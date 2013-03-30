const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

module.exports.setUp = function () {
  test('setUp', testCommon.setUp)
}

module.exports.close = function (leveldown) {
  test('test close()', function (t) {
    var db = leveldown(testCommon.location())

    db.open(function (err) {
      t.notOk(err, 'no error')
      t.throws(
          db.close.bind(db)
        , { name: 'Error', message: 'close() requires a callback argument' }
        , 'no-arg close() throws'
      )
      t.throws(
          db.close.bind(db, 'foo')
        , { name: 'Error', message: 'close() requires a callback argument' }
        , 'non-callback close() throws'
      )

      db.close(function (err) {
        t.notOk(err, 'no error')
        t.end()
      })
    })
  })
}

module.exports.tearDown = function () {
  test('tearDown', testCommon.tearDown)
}

module.exports.all = function (leveldown) {
  module.exports.setUp()
  module.exports.close(leveldown)
  module.exports.tearDown()
}

if (require.main === module)
  module.exports.all(leveldown)
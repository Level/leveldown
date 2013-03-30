const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

module.exports.setUp = function (leveldown) {
  test('setUp common', testCommon.setUp)
  test('setUp db', function (t) {
    db = leveldown(testCommon.location())
    db.open(t.end.bind(t))
  })
}

module.exports.args = function () {
  test('test argument-less del() throws', function (t) {
    t.throws(
        db.del.bind(db)
      , { name: 'Error', message: 'del() requires a callback argument' }
      , 'no-arg del() throws'
    )
    t.end()
  })

  test('test callback-less, 1-arg, del() throws', function (t) {
    t.throws(
        db.del.bind(db, 'foo')
      , { name: 'Error', message: 'del() requires a callback argument' }
      , 'callback-less, 1-arg del() throws'
    )
    t.end()
  })

  test('test callback-less, 3-arg, del() throws', function (t) {
    t.throws(
        db.del.bind(db, 'foo', {})
      , { name: 'Error', message: 'del() requires a callback argument' }
      , 'callback-less, 2-arg del() throws'
    )
    t.end()
  })
}

module.exports.del = function () {
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
}

module.exports.tearDown = function () {
  test('tearDown', function (t) {
    db.close(testCommon.tearDown.bind(null, t))
  })
}

module.exports.all = function (leveldown) {
  module.exports.setUp(leveldown)
  module.exports.args()
  module.exports.del()
  module.exports.tearDown()
}

if (require.main === module)
  module.exports.all(leveldown)
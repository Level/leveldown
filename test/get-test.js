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
  test('test argument-less get() throws', function (t) {
    t.throws(
        db.get.bind(db)
      , { name: 'Error', message: 'get() requires a callback argument' }
      , 'no-arg get() throws'
    )
    t.end()
  })

  test('test callback-less, 1-arg, get() throws', function (t) {
    t.throws(
        db.get.bind(db, 'foo')
      , { name: 'Error', message: 'get() requires a callback argument' }
      , 'callback-less, 1-arg get() throws'
    )
    t.end()
  })

  test('test callback-less, 3-arg, get() throws', function (t) {
    t.throws(
        db.get.bind(db, 'foo', {})
      , { name: 'Error', message: 'get() requires a callback argument' }
      , 'callback-less, 2-arg get() throws'
    )
    t.end()
  })
}

module.exports.get = function () {
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
}

module.exports.tearDown = function () {
  test('tearDown', function (t) {
    db.close(testCommon.tearDown.bind(null, t))
  })
}

module.exports.all = function (leveldown) {
  module.exports.setUp(leveldown)
  module.exports.args()
  module.exports.get()
  module.exports.tearDown()
}

if (require.main === module)
  module.exports.all(leveldown)
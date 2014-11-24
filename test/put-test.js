const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , abstract   = require('abstract-leveldown/abstract/put-test')

var db
    ,isTypedArray        = require('abstract-leveldown/abstract/util').isTypedArray

if (require.main === module) {
  var oldTearDown = abstract.tearDown
  abstract.tearDown = function (test, testCommon) {
    oldTearDown(test, testCommon)
    test('tearDown', function (t) {
      db.close(testCommon.tearDown.bind(null, t))
    })
  }
  abstract.args = function (test) {
    test('setUp common', testCommon.setUp)
    test('setUp db', function (t) {
      db = leveldown(testCommon.location())
      db.open(t.end.bind(t))
    })

    test('test argument-less put() throws', function (t) {
      t.throws(
          db.put.bind(db)
        , { name: 'Error', message: 'key cannot be `null` or `undefined`' }
        , 'no-arg put() throws'
      )
      t.end()
    })

    test('test callback-less, 1-arg, put() sync', function (t) {
      t.ok(db.put('foo'), "put key without value")
      t.end()
    })

    test('test callback-less, 2-arg, put() sync', function (t) {
      t.ok(db.put('foo', 'bar'), "put key value")
      t.end()
    })

    test('test callback-less, 3-arg, put() throws', function (t) {
      t.ok(db.put('foo', 'bar', {}), "put key value with empty options")
      t.end()
    })
  }
  abstract.all(leveldown, test, testCommon)
}
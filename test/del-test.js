const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , abstract   = require('abstract-leveldown/abstract/del-test')

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
    test('test argument-less del() throws', function (t) {
      t.throws(
          db.del.bind(db)
        , { name: 'Error', message: 'key cannot be `null` or `undefined`' }
        , 'no-arg del() throws'
      )
      t.end()
    })

    test('test callback-less, 1-arg, del() sync', function (t) {
      t.ok(db.del('foo'), "should delete key")
      t.end()
    })

    test('test callback-less, 3-arg, del() sync', function (t) {
      t.ok(db.del('foo', {}), "should delete key with empty options")
      t.end()
    })
  }
  abstract.all(leveldown, test, testCommon)
}

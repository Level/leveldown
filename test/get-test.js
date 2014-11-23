const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , getTest    = require('abstract-leveldown/abstract/get-test')

var db
    ,isTypedArray        = require('abstract-leveldown/abstract/util').isTypedArray


if (require.main === module) {
  var oldTearDown = getTest.tearDown
  getTest.tearDown = function (test, testCommon) {
    oldTearDown(test, testCommon)
    test('tearDown', function (t) {
      db.close(testCommon.tearDown.bind(null, t))
    })
  }

  var oldGetTest = getTest.get
  getTest.get = function (test) {
    oldGetTest(test)
    function testGet(t, aValue, expected) {
        t.ok(typeof aValue !== 'string', 'should not be string by default')
        var result
        if (isTypedArray(aValue)) {
          result = String.fromCharCode.apply(null, new Uint16Array(aValue))
        } else {
          t.ok(typeof Buffer != 'undefined' && aValue instanceof Buffer)
          result = aValue.toString()
        }
        t.equal(result, expected)
    }
    test('test simple get() sync', function (t) {
      db.put('fooSync', 'SbarSync', function(err){
        t.notOk(err, 'no error')
        var value = db.get('fooSync')
        testGet(t, value, 'SbarSync')
        value = db.get('fooSync', {}) // same but with {}
        testGet(t, value, 'SbarSync')
        value = db.get('fooSync', { asBuffer: false })
        t.ok(typeof value === 'string', 'should be string if not buffer')
        t.equal(value, 'SbarSync')
        t.end()
      })

    })

  }
  getTest.args = function (test) {
    test('setUp common', testCommon.setUp)
    test('setUp db', function (t) {
      db = leveldown(testCommon.location())
      db.open(t.end.bind(t))
    })
    test('test argument-less get() throws', function (t) {
      t['throws'](
          db.get.bind(db)
        , { name: 'Error', message: 'key cannot be `null` or `undefined`' }
        , 'no-arg get() throws'
      )
      t.end()
    })

    test('test callback-less, 1-arg, get() throws', function (t) {
      t['throws'](
          db.get.bind(db, 'fooErr')
        , { name: 'Error', message: 'NotFound: ' }
        , 'callback-less, 1-arg get() throws'
      )
      t.end()
    })

    test('test callback-less, 3-arg, get() throws', function (t) {
      t['throws'](
          db.get.bind(db, 'foo', {})
        , { name: 'Error', message: 'NotFound: ' }
        , 'callback-less, 2-arg get() throws'
      )
      t.end()
    })
  }

  getTest.all(leveldown, test, testCommon)
}

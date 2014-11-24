const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , abstract   = require('abstract-leveldown/abstract/batch-test')

var db
    ,isTypedArray        = require('abstract-leveldown/abstract/util').isTypedArray


function testGet(t, db, key, value) {
  var result = db.get(key)
  if (isTypedArray(result)) {
    result = String.fromCharCode.apply(null, new Uint16Array(result))
  } else {
    t.ok(typeof Buffer != 'undefined' && result instanceof Buffer)
    result = result.toString()
  }
  t.equal(result, value)  
}
if (require.main === module) {
  var oldTearDown = abstract.tearDown
  abstract.tearDown = function (test, testCommon) {
    oldTearDown(test, testCommon)
    test('tearDown', function (t) {
      db.close(testCommon.tearDown.bind(null, t))
    })
  }
  var oldArgs = abstract.args
  abstract.args = function (test) {
    test('setUp common', testCommon.setUp)
    test('setUp db', function (t) {
      db = leveldown(testCommon.location())
      db.open(t.end.bind(t))
    })
    oldArgs(test)
    test('test batch() with empty array', function (t) {
      t.notOk(db.batch([]), "should batch with empty array return false")
      t.end()
    })

    test('test simple batch()', function (t) {
      t.ok(db.batch([{ type: 'put', key: 'foo', value: 'bar' }]))
      testGet(t, db, 'foo', 'bar')
      t.end()
    })

    test('test multiple batch()', function (t) {
      t.ok(db.batch([
          { type: 'put', key: 'foobatch1', value: 'bar1' }
        , { type: 'put', key: 'foobatch2', value: 'bar2' }
        , { type: 'put', key: 'foobatch3', value: 'bar3' }
        , { type: 'del', key: 'foobatch2' }
      ]), "should be batch")
      testGet(t, db, 'foobatch1', 'bar1')
      t.throws(db.get.bind(db, 'foobatch2'), { name: 'Error', message: 'NotFound: ' })
      testGet(t, db, 'foobatch3', 'bar3')
      t.end()
    })
  }
  abstract.all(leveldown, test, testCommon)
}

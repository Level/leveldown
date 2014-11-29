const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , abstract   = require('abstract-leveldown/abstract/open-test')

var db
    ,isTypedArray        = require('abstract-leveldown/abstract/util').isTypedArray

if (require.main === module) {
  abstract.args = function (test) {
    test('setUp common', testCommon.setUp)
    test('setUp db', function (t) {
      db = leveldown(testCommon.location())
      db.open(t.end.bind(t))
    })
    test('test database open no-arg sync', function (t) {
      var db = leveldown(testCommon.location())
      t.ok(db.open(), "should be ok")
      t.end()
    })

    test('test callback-less, 1-arg, open() sync', function (t) {
      var db = leveldown(testCommon.location())
      t.ok(db.open({}), "should be ok")
      t.end()
      t.end()
    })
  }
  abstract.all(leveldown, test, testCommon)
}

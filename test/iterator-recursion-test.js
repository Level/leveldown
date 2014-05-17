const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')

var db
  , sourceData = (function () {
      var d = []
        , i = 0
        , k
      for (; i <  100000; i++) {
        k = (i < 10 ? '0' : '') + i
        d.push({
            type  : 'put'
          , key   : k
          , value : Math.random()
        })
      }
      return d
    }())

test('setUp common', testCommon.setUp)

test('setUp db', function (t) {
  db = leveldown(testCommon.location())
  db.open(function () {
    db.batch(sourceData, t.end.bind(t))
  })
})

test('iterate over a large iterator with a large watermark', function (t) {
  var iterator = db.iterator({
        highWaterMark: 10000000
    })
    , count = 0
    , read = function () {
        iterator.next(function () {
          count++

          if (!arguments.length)
            t.end()
          else
            read()
        })
      }

  read()
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})
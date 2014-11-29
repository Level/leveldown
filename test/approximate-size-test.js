const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    //, abstract   = require('abstract-leveldown/abstract/approximate-size-test')


var db
module.exports.setUp = function (leveldown, test, testCommon) {
  test('setUp common', testCommon.setUp)
  test('setUp db', function (t) {
    db = leveldown(testCommon.location())
    db.open(t.end.bind(t))
  })
}

module.exports.args = function (test) {
  test('test argument-less approximateSize() throws', function (t) {
    t.throws(
        db.approximateSize.bind(db)
      , { name: 'Error', message: 'approximateSize() requires valid `start`, `end` arguments' }
      , 'no-arg approximateSize() throws'
    )
    t.end()
  })

  test('test callback-less, 1-arg, approximateSize() throws', function (t) {
    t.throws(
        db.approximateSize.bind(db, 'foo')
      , { name: 'Error', message: 'approximateSize() requires valid `start`, `end` arguments' }
      , 'callback-less, 1-arg approximateSize() throws'
    )
    t.end()
  })

  test('test callback-less, 3-arg, approximateSize() throws', function (t) {
    t.throws(
        db.approximateSize.bind(db, function () {})
      , { name: 'Error', message: 'approximateSize() requires valid `start`, `end` arguments' }
      , 'callback-only approximateSize() throws'
    )
    t.end()
  })

  test('test callback-only approximateSize() throws', function (t) {
    t.throws(
        db.approximateSize.bind(db, function () {})
      , { name: 'Error', message: 'approximateSize() requires valid `start`, `end` arguments' }
      , 'callback-only approximateSize() throws'
    )
    t.end()
  })

  test('test 1-arg + callback approximateSize() throws', function (t) {
    t.throws(
        db.approximateSize.bind(db, 'foo', function () {})
      , { name: 'Error', message: 'approximateSize() requires valid `start`, `end` arguments' }
      , '1-arg + callback approximateSize() throws'
    )
    t.end()
  })
}

module.exports.approximateSize = function (test) {
  test('test approximateSize()', function (t) {
    var data = Array.apply(null, Array(10000)).map(function () {
      return 'aaaaaaaaaa'
    }).join('')

    db.batch(
        Array.apply(null, Array(10)).map(function (x, i) {
          return { type: 'put', key: 'foo' + i, value: data }
        })
      , function (err) {
          t.notOk(err, 'no error')

          // cycle open/close to ensure a pack to .sst

          db.close(function (err) {
            t.notOk(err, 'no error')

            db.open(function (err) {
              t.notOk(err, 'no error')
              var size = db.approximateSize('!', '~')
              t.type(size, 'number')
              t.ok(
                  size > 40000 // account for snappy compression
                               // original would be ~100000
                , 'size reports a reasonable amount (' + size + ') Sync'
              )


              db.approximateSize('!', '~', function (err, size) {
                t.notOk(err, 'no error')

                t.type(size, 'number')
                t.ok(
                    size > 40000 // account for snappy compression
                                 // original would be ~100000
                  , 'size reports a reasonable amount (' + size + ')'
                )

                db.close(function (err) {
                  t.notOk(err, 'no error')
                  t.end()
                })
              })
            })
          })
        }
    )
  })
}

module.exports.tearDown = function (test, testCommon) {
  test('tearDown', function (t) {
    db.close(testCommon.tearDown.bind(null, t))
  })
}

module.exports.all = function (leveldown, test, testCommon) {
  module.exports.setUp(leveldown, test, testCommon)
  module.exports.args(test)
  module.exports.approximateSize(test)
  module.exports.tearDown(test, testCommon)
}
abstract = module.exports

if (require.main === module) {
  abstract.all(leveldown, test, testCommon)
}

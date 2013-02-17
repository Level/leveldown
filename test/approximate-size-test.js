const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

test('setUp', testCommon.setUp)

test('test approximateSize()', function (t) {
  var db   = leveldown(testCommon.location())
    , data = Array.apply(null, Array(10000)).map(function () { return 'aaaaaaaaaa' }).join('')

  db.open(function (err) {
    t.notOk(err, 'no error')

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

              t.throws(db.approximateSize, 'arg-less approximateSize() throws')
              t.throws(
                  db.approximateSize.bind(db, 'foo')
                , '1-arg approximateSize() throws'
              )
              t.throws(
                  db.approximateSize.bind(db, 'foo', 'bar')
                , '2-arg approximateSize() throws'
              )
              t.throws(
                  db.approximateSize.bind(db, function () {})
                , 'callback-only approximateSize() throws'
              )
              t.throws(
                  db.approximateSize.bind(db, 'foo', function () {})
                , '1-arg + callback approximateSize() throws'
              )

              db.approximateSize('foo', 'z', function (err, size) {
                t.notOk(err, 'no error')

                t.type(size, 'number')
                t.ok(size > 100000, 'size reports a reasonable amount (' + size + ')')

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
})

test('tearDown', testCommon.tearDown)
const test = require('tape')
const testCommon = require('./common')

const key1 = '000000'
const key2 = '000001'
const val1 = Buffer.allocUnsafe(64).fill(1)
const val2 = Buffer.allocUnsafe(64).fill(1)

test('setUp', testCommon.setUp)

make('test compactRange() frees disk space after key deletion', function (db, t, done) {
  db.batch().put(key1, val1).put(key2, val2).write(function (err) {
    t.ifError(err, 'no batch put error')

    db.compactRange(key1, key2, function (err) {
      t.ifError(err, 'no compactRange1 error')

      db.approximateSize('0', 'z', function (err, sizeAfterPuts) {
        t.error(err, 'no approximateSize1 error')

        db.batch().del(key1).del(key2).write(function (err) {
          t.ifError(err, 'no batch del error')

          db.compactRange(key1, key2, function (err) {
            t.ifError(err, 'no compactRange2 error')

            db.approximateSize('0', 'z', function (err, sizeAfterCompact) {
              t.error(err, 'no approximateSize2 error')
              t.ok(sizeAfterCompact < sizeAfterPuts)
              done()
            })
          })
        })
      })
    })
  })
})

make('test compactRange() serializes start and end', function (db, t, done) {
  var count = 0

  db._serializeKey = function (key) {
    t.is(key, count++)
    return String(key)
  }

  db.compactRange(0, 1, function (err) {
    t.ifError(err, 'no compactRange error')
    t.is(count, 2, '_serializeKey called twice')

    done()
  })
})

make('test compactRange() throws if callback is missing', function (db, t, done) {
  try {
    db.compactRange()
  } catch (err) {
    t.is(err.message, 'compactRange() requires a callback argument')
    done()
  }
})

make('test compactRange() without end', function (db, t, done) {
  db.batch().put(key1, val1).put(key2, val2).write(function (err) {
    t.ifError(err, 'no batch put error')

    db.compactRange(key1, function (err) {
      t.ifError(err, 'no compactRange1 error')

      db.approximateSize('0', 'z', function (err, sizeAfterPuts) {
        t.error(err, 'no approximateSize1 error')

        db.batch().del(key1).del(key2).write(function (err) {
          t.ifError(err, 'no batch del error')

          db.compactRange(key1, function (err) {
            t.ifError(err, 'no compactRange2 error')

            db.approximateSize('0', 'z', function (err, sizeAfterCompact) {
              t.error(err, 'no approximateSize2 error')
              t.ok(sizeAfterCompact < sizeAfterPuts)
              done()
            })
          })
        })
      })
    })
  })
})

make('test compactRange() without start and end', function (db, t, done) {
  db.batch().put(key1, val1).put(key2, val2).write(function (err) {
    t.ifError(err, 'no batch put error')

    db.compactRange(function (err) {
      t.ifError(err, 'no compactRange1 error')

      db.approximateSize('0', 'z', function (err, sizeAfterPuts) {
        t.error(err, 'no approximateSize1 error')

        db.batch().del(key1).del(key2).write(function (err) {
          t.ifError(err, 'no batch del error')

          db.compactRange(function (err) {
            t.ifError(err, 'no compactRange2 error')

            db.approximateSize('0', 'z', function (err, sizeAfterCompact) {
              t.error(err, 'no approximateSize2 error')
              t.ok(sizeAfterCompact < sizeAfterPuts)
              done()
            })
          })
        })
      })
    })
  })
})

make('test compactRange() outside of key space', function (db, t, done) {
  db.batch().put(key1, val1).put(key2, val2).write(function (err) {
    t.ifError(err, 'no batch put error')

    db.compactRange(function (err) {
      t.ifError(err, 'no compactRange1 error')

      db.approximateSize('0', 'z', function (err, sizeAfterPuts) {
        t.error(err, 'no approximateSize1 error')

        db.batch().del(key1).del(key2).write(function (err) {
          t.ifError(err, 'no batch del error')

          db.compactRange('z', function (err) {
            t.ifError(err, 'no compactRange2 error')

            db.approximateSize('0', 'z', function (err, sizeAfterCompact) {
              t.error(err, 'no approximateSize2 error')
              t.ok(sizeAfterCompact >= sizeAfterPuts, 'compactRange did nothing')
              done()
            })
          })
        })
      })
    })
  })
})

test('tearDown', testCommon.tearDown)

function make (name, testFn) {
  test(name, function (t) {
    var db = testCommon.factory()
    var done = function (err) {
      t.ifError(err, 'no done error')

      db.close(function (err) {
        t.ifError(err, 'no error from close()')
        t.end()
      })
    }

    db.open(function (err) {
      t.ifError(err, 'no error from open()')
      testFn(db, t, done)
    })
  })
}

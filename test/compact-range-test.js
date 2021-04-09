const test = require('tape')
const testCommon = require('./common')

let db

test('setUp common', testCommon.setUp)

test('setUp db', function (t) {
  db = testCommon.factory()
  db.open(t.end.bind(t))
})

test('test compactRange() frees disk space after key deletion', function (t) {
  const key1 = '000000'
  const key2 = '000001'
  const val1 = Buffer.allocUnsafe(64).fill(1)
  const val2 = Buffer.allocUnsafe(64).fill(1)

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
              t.end()
            })
          })
        })
      })
    })
  })
})

test('test compactRange() serializes start and end', function (t) {
  t.plan(3)

  const clone = Object.create(db)
  let count = 0

  clone._serializeKey = function (key) {
    t.is(key, count++)
    return db._serializeKey(key)
  }

  clone.compactRange(0, 1, function (err) {
    t.ifError(err, 'no compactRange error')
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})

const test = require('tape')
const testCommon = require('./common')
const operations = []

// The db must wait for pending operations to finish before closing. This to
// prevent segfaults and in the case of compactRange() to prevent hanging. See
// https://github.com/Level/leveldown/issues/157 and 32.
function testPending (name, expectedCount, fn) {
  operations.push(fn)

  test(`close() waits for pending ${name}`, function (t) {
    const db = testCommon.factory()
    let count = 0

    db.open(function (err) {
      t.ifError(err, 'no error from open()')

      db.put('key', 'value', function (err) {
        t.ifError(err, 'no error from put()')

        fn(db, function (err) {
          count++
          t.ifError(err, 'no error from operation')
        })

        db.close(function (err) {
          t.ifError(err, 'no error from close()')
          t.is(count, expectedCount, 'operation(s) finished before close')
          t.end()
        })
      })
    })
  })
}

testPending('get()', 1, function (db, next) {
  db.get('key', next)
})

testPending('put()', 1, function (db, next) {
  db.put('key2', 'value', next)
})

testPending('put() with { sync }', 1, function (db, next) {
  // The sync option makes the operation slower and thus more likely to
  // cause a segfault (if closing were to happen during the operation).
  db.put('key2', 'value', { sync: true }, next)
})

testPending('del()', 1, function (db, next) {
  db.del('key', next)
})

testPending('del() with { sync }', 1, function (db, next) {
  db.del('key', { sync: true }, next)
})

testPending('batch([])', 1, function (db, next) {
  db.batch([{ type: 'del', key: 'key' }], next)
})

testPending('batch([]) with { sync }', 1, function (db, next) {
  db.batch([{ type: 'del', key: 'key' }], { sync: true }, next)
})

testPending('batch()', 1, function (db, next) {
  db.batch().del('key').write(next)
})

testPending('batch() with { sync }', 1, function (db, next) {
  db.batch().del('key').write({ sync: true }, next)
})

testPending('approximateSize()', 1, function (db, next) {
  db.approximateSize('a', 'z', next)
})

testPending('compactRange()', 1, function (db, next) {
  db.compactRange('a', 'z', next)
})

// Test multiple pending operations, using all of the above.
testPending('operations', operations.length, function (db, next) {
  for (const fn of operations.slice(0, -1)) {
    fn(db, next)
  }
})

// See https://github.com/Level/leveldown/issues/134
test('iterator() does not segfault if db is not open', function (t) {
  t.plan(2)

  const db = testCommon.factory()

  try {
    db.iterator()
  } catch (err) {
    t.is(err.message, 'cannot call iterator() before open()')
  }

  db.close(function (err) {
    t.ifError(err, 'no close error')
  })
})

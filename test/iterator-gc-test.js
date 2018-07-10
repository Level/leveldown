'use strict'

const test = require('tape')
const testCommon = require('abstract-leveldown/test/common')
const leveldown = require('..')
const sourceData = []

for (let i = 0; i < 1e3; i++) {
  sourceData.push({
    type: 'put',
    key: i.toString(),
    value: Math.random().toString()
  })
}

test('setUp', testCommon.setUp)

// When you have a database open with an active iterator, but no references to
// the db, V8 will GC the database and you'll get an failed assert from LevelDB.
test('db without ref does not get GCed while iterating', function (t) {
  t.plan(6)

  let db = leveldown(testCommon.location())

  db.open(function (err) {
    t.ifError(err, 'no open error')

    // Insert test data
    db.batch(sourceData.slice(), function (err) {
      t.ifError(err, 'no batch error')

      // Set highWaterMark to 0 so that we don't preemptively fetch.
      const it = db.iterator({ highWaterMark: 0 })

      // Remove reference
      db = null

      if (global.gc) {
        // This is the reliable way to trigger GC (and the bug if it exists).
        // Useful for manual testing with "node --expose-gc".
        global.gc()
        iterate(it)
      } else {
        // But a timeout usually also allows GC to kick in. If not, the time
        // between iterator ticks might. That's when "highWaterMark: 0" helps.
        setTimeout(iterate.bind(null, it), 1000)
      }
    })
  })

  function iterate (it) {
    // No reference to db here, could be GCed. It shouldn't..
    testCommon.collectEntries(it, function (err, entries) {
      t.ifError(err, 'no iterator error')
      t.is(entries.length, sourceData.length, 'got data')

      // Because we also have a reference on the iterator. That's the fix.
      t.ok(it.db, 'abstract iterator has reference to db')

      // Which as luck would have it, also allows us to properly end this test.
      it.db.close(function (err) {
        t.ifError(err, 'no close error')
      })
    })
  }
})

test('tearDown', testCommon.tearDown)

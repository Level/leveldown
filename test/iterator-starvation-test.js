'use strict'

const test = require('tape')
const testCommon = require('./common')
const sourceData = []

// For this test the number of records in the db must be a multiple of
// the hardcoded fast-future limit (1000) or a cache size limit in C++.
for (let i = 0; i < 1e4; i++) {
  sourceData.push({
    type: 'put',
    key: i.toString(),
    value: ''
  })
}

test('setUp', testCommon.setUp)

test('iterator does not starve event loop', function (t) {
  t.plan(6)

  const db = testCommon.factory()

  db.open(function (err) {
    t.ifError(err, 'no open error')

    // Insert test data
    db.batch(sourceData.slice(), function (err) {
      t.ifError(err, 'no batch error')

      // Set a high highWaterMark to fill up the cache entirely
      const it = db.iterator({ highWaterMark: Math.pow(1024, 3) })

      let breaths = 0
      let entries = 0
      let scheduled = false

      // Iterate continously while also scheduling work with setImmediate(),
      // which should be given a chance to run because we limit the tick depth.
      const next = function () {
        it.next(function (err, key, value) {
          if (err || (key === undefined && value === undefined)) {
            t.ifError(err, 'no next error')
            t.is(entries, sourceData.length, 'got all data')
            t.ok(breaths > 1, 'breathed while iterating: ' + breaths)

            // More precise assertion (only works for cache size limit, not fast-future):
            // t.is(breaths, sourceData.length / 1000, 'breathed while iterating')

            return db.close(function (err) {
              t.ifError(err, 'no close error')
            })
          }

          entries++

          if (!scheduled) {
            scheduled = true
            setImmediate(function () {
              breaths++
              scheduled = false
            })
          }

          next()
        })
      }

      next()
    })
  })
})

test('tearDown', testCommon.tearDown)

'use strict'

const test = require('tape')
const testCommon = require('./common')

// When we have a chained batch object without a reference, V8 might GC it
// before we get a chance to (asynchronously) write the batch.
test('chained batch without ref does not get GCed before write', function (t) {
  t.plan(2)

  const db = testCommon.factory()

  db.open(function (err) {
    t.ifError(err, 'no open error')

    let batch = db.batch()

    for (let i = 0; i < 1e3; i++) {
      batch.put(String(i), 'value')
    }

    // The sync option makes the operation slower and thus more likely to
    // cause a segfault (if the batch were to be GC-ed before it is written).
    batch.write({ sync: true }, function (err) {
      t.ifError(err, 'no error from write()')
    })

    // Remove reference
    batch = null

    if (global.gc) {
      // This is the reliable way to trigger GC (and the bug if it exists).
      // Useful for manual testing with "node --expose-gc".
      global.gc()
    }
  })
})

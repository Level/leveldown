'use strict'

const test = require('tape')
const testCommon = require('./common')
const sourceData = []

for (let i = 0; i < 1e3; i++) {
  sourceData.push({
    type: 'put',
    key: i.toString(),
    value: Math.random().toString()
  })
}

test('db without ref does not get GCed while clear() is in progress', function (t) {
  t.plan(4)

  let db = testCommon.factory()

  db.open(function (err) {
    t.ifError(err, 'no open error')

    // Insert test data
    db.batch(sourceData.slice(), function (err) {
      t.ifError(err, 'no batch error')

      // Start async work
      db.clear(function () {
        t.pass('got callback')

        // Give GC another chance to run, to rule out other issues.
        setImmediate(function () {
          if (global.gc) global.gc()
          t.pass()
        })
      })

      // Remove reference. The db should not get garbage collected
      // until after the clear() callback, thanks to a napi_ref.
      db = null

      // Useful for manual testing with "node --expose-gc".
      // The pending tap assertion may also allow GC to kick in.
      if (global.gc) global.gc()
    })
  })
})

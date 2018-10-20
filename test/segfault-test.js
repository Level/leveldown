const test = require('tape')
const testCommon = require('./common')

// Open issue: https://github.com/Level/leveldown/issues/157
test.skip('close() does not segfault if there is a pending write', function (t) {
  t.plan(3)

  const db = testCommon.factory()

  db.open(function (err) {
    t.ifError(err, 'no open error')

    // The "sync" option seems to be a reliable way to trigger a segfault,
    // but is not necessarily the cause of that segfault. More likely, it
    // exposes a race condition that's already there.
    db.put('foo', 'bar', { sync: true }, function (err) {
      // We never get here, due to segfault.
      t.ifError(err, 'no put error')
    })

    db.close(function (err) {
      // We never get here, due to segfault.
      t.ifError(err, 'no close error')
    })
  })
})

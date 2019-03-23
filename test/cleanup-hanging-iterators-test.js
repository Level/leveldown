const makeTest = require('./make')
const repeats = 200

makeTest('test legacy ended iterator', function (db, t, done) {
  // First test the old proper usage: calling it.end() before db.close().
  var it = db.iterator({ keyAsBuffer: false, valueAsBuffer: false })

  it.next(function (err, key, value) {
    t.ifError(err, 'no error from next()')
    t.equal(key, 'one', 'correct key')
    t.equal(value, '1', 'correct value')
    it.end(function (err) {
      t.ifError(err, 'no error from end()')
      done()
    })
  })
})

makeTest('test likely-ended iterator', function (db, t, done) {
  // Test simpler usage: not calling it.end() before db.close(). Cleanup of the
  // database will crash Node if not handled properly.
  var it = db.iterator({ keyAsBuffer: false, valueAsBuffer: false })

  it.next(function (err, key, value) {
    t.ifError(err, 'no error from next()')
    t.equal(key, 'one', 'correct key')
    t.equal(value, '1', 'correct value')
    done()
  })
})

makeTest('test non-ended iterator', function (db, t, done) {
  // Same as the test above but with a highWaterMark of 0 so that we don't
  // preemptively fetch all records, to ensure that the iterator is still
  // active when we (attempt to) close the database.
  var it = db.iterator({
    highWaterMark: 0,
    keyAsBuffer: false,
    valueAsBuffer: false
  })

  it.next(function (err, key, value) {
    t.ifError(err, 'no error from next()')
    t.equal(key, 'one', 'correct key')
    t.equal(value, '1', 'correct value')
    done()
  })
})

makeTest('test multiple likely-ended iterators', function (db, t, done) {
  // Same as the test above but repeated and with an extra iterator that is not
  // nexting, which follows a different (faster) code path.
  for (let i = 0; i < repeats; i++) {
    db.iterator()
    db.iterator().next(function () {})
  }

  setTimeout(done, Math.floor(Math.random() * 50))
})

makeTest('test multiple non-ended iterators', function (db, t, done) {
  // Same as the test above but with a highWaterMark of 0.
  for (let i = 0; i < repeats; i++) {
    db.iterator({ highWaterMark: 0 })
    db.iterator({ highWaterMark: 0 }).next(function () {})
  }

  setTimeout(done, Math.floor(Math.random() * 50))
})

global.gc && makeTest('test multiple non-ended iterators with forced gc', function (db, t, done) {
  // Same as the test above but with forced GC, to test that the lifespan of an
  // iterator is tied to *both* its JS object and whether it's busy (nexting).
  for (let i = 0; i < repeats; i++) {
    db.iterator({ highWaterMark: 0 })
    db.iterator({ highWaterMark: 0 }).next(function () {})
  }

  setTimeout(function () {
    global.gc()
    done()
  }, Math.floor(Math.random() * 50))
})

global.gc && makeTest('test multiple iterators with forced gc in next()', function (db, t, done) {
  // Simulate GC in between NextWorker::DoComplete() and ::DoFinally().
  for (let i = 0; i < 10; i++) {
    db.iterator({ highWaterMark: 0 }).next(function () {
      global.gc()
    })
  }

  setImmediate(done)
})

makeTest('test legacy ended iterators', function (db, t, done) {
  // This test doesn't have added value anymore.
  var it1 = db.iterator().next(function () {
    it1.end(function () {})
  })
  var it2 = db.iterator().next(function () {
    it2.end(function () {})
    done()
  })
})

makeTest('test accessing an iterator after close', function (db, t, done) {
  var it = db.iterator({ highWaterMark: 0 })

  db.close(function (err) {
    t.ifError(err, 'no error from close()')

    // Because we have a reference here, the iterator is not GC-ed yet, only
    // ended. The object should still be safe to access.
    it.next(function (err) {
      t.is(err.message, 'iterator has ended')
      done(null, false)
    })
  })
})

makeTest('test accessing an iterator after close and reopen', function (db, t, done) {
  var it = db.iterator({ highWaterMark: 0 })

  db.close(function (err) {
    t.ifError(err, 'no error from close()')

    db.open(function (err) {
      t.ifError(err, 'no error from open()')

      // The state of an iterator should not be tied to the open-state of the
      // database, but to the singular database "life" of when it was created.
      it.next(function (err) {
        t.is(err.message, 'iterator has ended')
        done()
      })
    })
  })
})

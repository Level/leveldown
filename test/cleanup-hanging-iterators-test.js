const makeTest = require('./make')
const repeats = 200

makeTest('test ended iterator', function (db, t, done) {
  // First test normal and proper usage: calling it.end() before db.close()
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
  // Test improper usage: not calling it.end() before db.close(). Cleanup of the
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
  // nexting, which means its EndWorker will be executed almost immediately.
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
  // iterator is tied to *both* its JS object and whether the iterator was ended.
  for (let i = 0; i < repeats; i++) {
    db.iterator({ highWaterMark: 0 })
    db.iterator({ highWaterMark: 0 }).next(function () {})
  }

  setTimeout(function () {
    global.gc()
    done()
  }, Math.floor(Math.random() * 50))
})

makeTest('test ending iterators', function (db, t, done) {
  // At least one end() should be in progress when we try to close the db.
  var it1 = db.iterator().next(function () {
    it1.end(function () {})
  })
  var it2 = db.iterator().next(function () {
    it2.end(function () {})
    done()
  })
})

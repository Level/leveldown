const test = require('tape')
const testCommon = require('./common')

let db

test('setUp common for approximate size', testCommon.setUp)

test('setUp db', function (t) {
  db = testCommon.factory()
  db.open(t.end.bind(t))
})

test('test argument-less approximateSize() throws', function (t) {
  t.throws(
    db.approximateSize.bind(db)
    , /^Error: approximateSize\(\) requires valid `start` and `end` arguments/
    , 'no-arg approximateSize() throws'
  )
  t.end()
})

test('test callback-less, 1-arg, approximateSize() throws', function (t) {
  t.throws(
    db.approximateSize.bind(db, 'foo')
    , /^Error: approximateSize\(\) requires valid `start` and `end` arguments/
    , 'callback-less, 1-arg approximateSize() throws'
  )
  t.end()
})

test('test callback-less, 2-arg, approximateSize() throws', function (t) {
  t.throws(
    db.approximateSize.bind(db, 'foo', 'bar')
    , /^Error: approximateSize\(\) requires a callback argument/
    , 'callback-less, 2-arg approximateSize() throws'
  )
  t.end()
})

test('test callback-less, 3-arg, approximateSize() throws', function (t) {
  t.throws(
    db.approximateSize.bind(db, function () {})
    , /^Error: approximateSize\(\) requires valid `start` and `end` arguments/
    , 'callback-only approximateSize() throws'
  )
  t.end()
})

test('test callback-only approximateSize() throws', function (t) {
  t.throws(
    db.approximateSize.bind(db, function () {})
    , /^Error: approximateSize\(\) requires valid `start` and `end` arguments/
    , 'callback-only approximateSize() throws'
  )
  t.end()
})

test('test 1-arg + callback approximateSize() throws', function (t) {
  t.throws(
    db.approximateSize.bind(db, 'foo', function () {})
    , /^Error: approximateSize\(\) requires valid `start` and `end` arguments/
    , '1-arg + callback approximateSize() throws'
  )
  t.end()
})

test('test custom _serialize*', function (t) {
  t.plan(4)
  const db = testCommon.factory()
  db._serializeKey = function (data) { return data }
  db.approximateSize = function (start, end, callback) {
    t.deepEqual(start, { foo: 'bar' })
    t.deepEqual(end, { beep: 'boop' })
    process.nextTick(callback)
  }
  db.open(function () {
    db.approximateSize({ foo: 'bar' }, { beep: 'boop' }, function (err) {
      t.error(err)
      db.close(t.error.bind(t))
    })
  })
})

test('test approximateSize()', function (t) {
  const data = Array.apply(null, Array(10000)).map(function () {
    return 'aaaaaaaaaa'
  }).join('')

  db.batch(Array.apply(null, Array(10)).map(function (x, i) {
    return { type: 'put', key: 'foo' + i, value: data }
  }), function (err) {
    t.error(err)

    // cycle open/close to ensure a pack to .sst

    db.close(function (err) {
      t.error(err)

      db.open(function (err) {
        t.error(err)

        db.approximateSize('!', '~', function (err, size) {
          t.error(err)

          t.equal(typeof size, 'number')
          // account for snappy compression, original would be ~100000
          t.ok(size > 40000, 'size reports a reasonable amount (' + size + ')')
          t.end()
        })
      })
    })
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})

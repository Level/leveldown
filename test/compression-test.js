const each = require('async-each')
const du = require('du')
const delayed = require('delayed')
const testCommon = require('./common')
const leveldown = require('..')
const test = require('tape')

const compressableData = Buffer.from(Array.apply(null, Array(1024 * 100)).map(function () {
  return 'aaaaaaaaaa'
}).join(''))

const multiples = 10
const dataSize = compressableData.length * multiples

const verify = function (location, compression, t) {
  du(location, function (err, size) {
    t.error(err)
    if (compression) {
      t.ok(size < dataSize, 'on-disk size (' + size + ') is less than data size (' + dataSize + ')')
    } else {
      t.ok(size >= dataSize, 'on-disk size (' + size + ') is greater than data size (' + dataSize + ')')
    }
    t.end()
  })
}

// close, open, close again.. 'compaction' is also performed on open()s
const cycle = function (db, compression, t, callback) {
  var location = db.location
  db.close(function (err) {
    t.error(err)
    db = leveldown(location)
    db.open({ errorIfExists: false, compression: compression }, function () {
      t.error(err)
      db.close(function (err) {
        t.error(err)
        callback()
      })
    })
  })
}

test('compression', function (t) {
  t.plan(4)
  t.test('set up', testCommon.setUp)

  t.test('test data is compressed by default (db.put())', function (t) {
    var db = testCommon.factory()
    db.open(function (err) {
      t.error(err)
      each(
        Array.apply(null, Array(multiples)).map(function (e, i) {
          return [i, compressableData]
        }), function (args, callback) {
          db.put.apply(db, args.concat([callback]))
        }, cycle.bind(null, db, true, t, delayed.delayed(verify.bind(null, db.location, true, t), 0.01))
      )
    })
  })

  t.test('test data is not compressed with compression=false on open() (db.put())', function (t) {
    var db = testCommon.factory()
    db.open({ compression: false }, function (err) {
      t.error(err)
      each(
        Array.apply(null, Array(multiples)).map(function (e, i) {
          return [i, compressableData]
        }), function (args, callback) {
          db.put.apply(db, args.concat([callback]))
        }, cycle.bind(null, db, false, t, delayed.delayed(verify.bind(null, db.location, false, t), 0.01))
      )
    })
  })

  t.test('test data is compressed by default (db.batch())', function (t) {
    var db = testCommon.factory()
    db.open(function (err) {
      t.error(err)
      db.batch(
        Array.apply(null, Array(multiples)).map(function (e, i) {
          return { type: 'put', key: i, value: compressableData }
        }), cycle.bind(null, db, true, t, delayed.delayed(verify.bind(null, db.location, true, t), 0.01))
      )
    })
  })
})

/* Copyright (c) 2012-2017 LevelUP contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

var async = require('async'),
  du = require('du'),
  delayed = require('delayed'),
  common = require('abstract-leveldown/testCommon'),
  leveldown = require('..'),
  test = require('tape'),

  compressableData = Buffer.from(Array.apply(null, Array(1024 * 100)).map(function () { return 'aaaaaaaaaa' }).join('')),
  multiples = 10,
  dataSize = compressableData.length * multiples,

  verify = function (location, compression, t) {
    du(location, function (err, size) {
      t.error(err)
      if (compression) { t.ok(size < dataSize, 'on-disk size (' + size + ') is less than data size (' + dataSize + ')') } else { t.ok(size >= dataSize, 'on-disk size (' + size + ') is greater than data size (' + dataSize + ')') }
      t.end()
    })
  },

  // close, open, close again.. 'compaction' is also performed on open()s
  cycle = function (db, compression, t, callback) {
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

test('Compression', function (t) {
  t.test('set up', common.setUp)

  t.test('test data is compressed by default (db.put())', function (t) {
    var db = leveldown(common.location())
    db.open(function (err) {
      t.error(err)
      async.forEach(
        Array.apply(null, Array(multiples)).map(function (e, i) {
          return [ i, compressableData ]
        })
        , function (args, callback) {
          db.put.apply(db, args.concat([callback]))
        }
        , cycle.bind(null, db, true, t, delayed.delayed(verify.bind(null, db.location, true, t), 0.01))
      )
    })
  })

  t.test('test data is not compressed with compression=false on open() (db.put())', function (t) {
    var db = leveldown(common.location())
    db.open({ compression: false }, function (err) {
      t.error(err)
      async.forEach(
        Array.apply(null, Array(multiples)).map(function (e, i) {
          return [ i, compressableData ]
        })
        , function (args, callback) {
          db.put.apply(db, args.concat([callback]))
        }
        , cycle.bind(null, db, false, t, delayed.delayed(verify.bind(null, db.location, false, t), 0.01))
      )
    })
  })

  t.test('test data is compressed by default (db.batch())', function (t) {
    var db = leveldown(common.location())
    db.open(function (err) {
      t.error(err)
      db.batch(
        Array.apply(null, Array(multiples)).map(function (e, i) {
          return { type: 'put', key: i, value: compressableData }
        })
        , cycle.bind(null, db, false, t, delayed.delayed(verify.bind(null, db.location, false, t), 0.01))
      )
    })
  })
})

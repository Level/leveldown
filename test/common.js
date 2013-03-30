const path      = require('path')
    , fs        = require('fs')
    , rimraf    = require('rimraf')
    , test      = require('tap').test
    , leveldown = require('../')

var dbidx = 0

  , location = function () {
      return path.join(__dirname, '_leveldown_test_db_' + dbidx++)
    }

  , lastLocation = function () {
      return path.join(__dirname, '_leveldown_test_db_' + dbidx)
    }

  , cleanup = function (callback) {
      fs.readdir(__dirname, function (err, list) {
        if (err) return callback(err)

        list = list.filter(function (f) {
          return (/^_leveldown_test_db_/).test(f)
        })

        if (!list.length)
          return callback()

        var ret = 0

        list.forEach(function (f) {
          rimraf(path.join(__dirname, f), function (err) {
            if (++ret == list.length)
              callback()
          })
        })
      })
    }

  , setUp = function (t) {
      cleanup(function (err) {
        t.notOk(err, 'cleanup returned an error')
        t.end()
      })
    }

  , tearDown = function (t) {
      setUp(t) // same cleanup!
    }

  , collectEntries = function (iterator, callback) {
      var data = []
        , next = function () {
            iterator.next(function (err, key, value) {
              if (err) return callback(err)
              if (!arguments.length) {
                return iterator.end(function (err) {
                  callback(err, data)
                })
              }
              if (key == +key) key = +key
              data.push({ key: key, value: value })
              process.nextTick(next)
            })
          }
      next()
    }

  , makeExistingDbTest = function (name, testFn) {
      test(name, function (t) {
        cleanup(function () {
          var loc  = location()
            , db   = leveldown(loc)
            , done = function (close) {
                if (close === false)
                  return cleanup(t.end.bind(t))
                db.close(function (err) {
                  t.notOk(err, 'no error from close()')
                  cleanup(t.end.bind(t))
                })
              }
          db.open(function (err) {
           t.notOk(err, 'no error from open()')
            db.batch(
                [
                    { type: 'put', key: 'one', value: '1' }
                  , { type: 'put', key: 'two', value: '2' }
                  , { type: 'put', key: 'three', value: '3' }
                ]
              , function (err) {
                  t.notOk(err, 'no error from batch()')
                  testFn(db, t, done, loc)
                }
            )
          })
        })
      })
    }

module.exports = {
    location       : location
  , cleanup        : cleanup
  , lastLocation   : lastLocation
  , setUp          : setUp
  , tearDown       : tearDown
  , collectEntries : collectEntries
  , makeExistingDbTest : makeExistingDbTest
}

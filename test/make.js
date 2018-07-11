const test = require('tape')
const testCommon = require('./common')
const leveldown = require('..')

function makeTest (name, testFn) {
  test(name, function (t) {
    var location = testCommon.location()
    var db = leveldown(location)
    var done = function (close) {
      if (close === false) {
        t.end()
        return
      }
      db.close(function (err) {
        t.error(err, 'no error from close()')
        t.end()
      })
    }
    db.open(function (err) {
      t.error(err, 'no error from open()')
      db.batch([
        { type: 'put', key: 'one', value: '1' },
        { type: 'put', key: 'two', value: '2' },
        { type: 'put', key: 'three', value: '3' }
      ], function (err) {
        t.error(err, 'no error from batch()')
        testFn(db, t, done, location)
      })
    })
  })
}

module.exports = makeTest

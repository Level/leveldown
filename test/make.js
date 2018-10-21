const test = require('tape')
const testCommon = require('./common')

function makeTest (name, testFn) {
  test(name, function (t) {
    var db = testCommon.factory()
    var done = function (err, close) {
      t.ifError(err, 'no error from done()')

      if (close === false) {
        t.end()
        return
      }

      db.close(function (err) {
        t.ifError(err, 'no error from close()')
        t.end()
      })
    }
    db.open(function (err) {
      t.ifError(err, 'no error from open()')
      db.batch([
        { type: 'put', key: 'one', value: '1' },
        { type: 'put', key: 'two', value: '2' },
        { type: 'put', key: 'three', value: '3' }
      ], function (err) {
        t.ifError(err, 'no error from batch()')
        testFn(db, t, done)
      })
    })
  })
}

module.exports = makeTest

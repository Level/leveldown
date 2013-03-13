const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

test('setUp', function (t) {
  db = leveldown(testCommon.location())
  db.open(testCommon.setUp.bind(null, t))
})

test('basic batch', function (t) {
  db.batch(
      [
          { type: 'put', key: 'one', value: '1' }
        , { type: 'put', key: 'two', value: '2' }
        , { type: 'put', key: 'three', value: '3' }
      ]
    , function (err) {
        t.notOk(err, 'no error')

        db.batch()
          .put('one', 'I')
          .put('two', 'II')
          .del('three')
          .put('foo', 'bar')
          .write(function (err) {
            t.notOk(err, 'no error')
            testCommon.collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false }), function (err, data) {
              t.notOk(err, 'no error')
              t.equal(data.length, 3, 'correct number of entries')
              var expected = [
                  { key: 'foo', value: 'bar' }
                , { key: 'one', value: 'I' }
                , { key: 'two', value: 'II' }
              ]
              t.deepEqual(data, expected)
              t.end()
            })
          })
      }
  )
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})
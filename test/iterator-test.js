const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

test('setUp', function (t) {
  db = leveldown(testCommon.location())
  db.open(testCommon.setUp.bind(null, t))
})

test('test argument-less iterator#next() throws', function (t) {
  var iterator = db.iterator()
  t.throws(iterator.next.bind(iterator), 'no-arg iterator#next() throws')
  iterator.end(t.end.bind(t))
})

test('test argument-less iterator#end() after next() throws', function (t) {
  var iterator = db.iterator()
  iterator.next(function () {
    t.throws(iterator.end.bind(iterator), 'no-arg iterator#end() throws')
    iterator.end(t.end.bind(t))
  })
})

test('test argument-less iterator#end() throws', function (t) {
  var iterator = db.iterator()
  t.throws(iterator.end.bind(iterator), 'no-arg iterator#end() throws')
  iterator.end(t.end.bind(t))
})

test('test twice iterator#end() throws', function (t) {
  var iterator = db.iterator()
  iterator.end(function (err) {
    t.notOk(err, 'no error')
    t.throws(iterator.end.bind(iterator, function () {}), 'twice iterator#end() throws')
    t.end()
  })
})

test('test twice iterator#next() throws', function (t) {
  var iterator = db.iterator()
  iterator.next(function (err) {
    t.notOk(err, 'no error')
    iterator.end(function (err) {
      t.notOk(err, 'no error')
      t.end()
    })
  })
  t.throws(iterator.next.bind(iterator, function () {}), 'twice iterator#next() throws')
})

test('test simple iterator()', function (t) {
  var data = [
          { type: 'put', key: 'foobatch1', value: 'bar1' }
        , { type: 'put', key: 'foobatch2', value: 'bar2' }
        , { type: 'put', key: 'foobatch3', value: 'bar3' }
      ]
    , idx = 0

  db.batch(data, function (err) {
    t.notOk(err, 'no error')

    var iterator = db.iterator()
      , fn = function (err, key, value) {
          t.notOk(err, 'no error')
          if (key && value) {
            t.equal(key.toString(), data[idx].key, 'correct key')
            t.equal(value.toString(), data[idx].value, 'correct value')
            process.nextTick(next)
            idx++
          } else { // end
            t.type(err, 'undefined', 'err argument is undefined')
            t.type(key, 'undefined', 'key argument is undefined')
            t.type(value, 'undefined', 'value argument is undefined')
            t.equal(idx, data.length, 'correct number of entries')
            iterator.end(function () {
              t.end()
            })
          }
        }
      , next = function () {
          iterator.next(fn)
        }

    next()
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})
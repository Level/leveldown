const fs         = require('fs')
    , path       = require('path')
    , test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

var db

  , makePutErrorTest = function (type, key, value, expectedError) {
      test('put() with ' + type + ' causes error', function (t) {
        db.put(key, value, function (err) {
          t.ok(err, 'has error')
          t.type(err, Error)
          t.like(err.message, expectedError, 'correct error message')
          t.end()
        })
      })
    }

  , makePutTest = function (t, key, value, callback) {
      db.put(key, value, function (err) {
        t.notOk(err, 'no error')
        db.get(key, function (err, _value) {
          t.notOk(err, 'no error, has key/value for `' + key + '`')
          t.equals(_value.toString(), value.toString())
          callback()
        })
      })
    }

test('setUp', function (t) {
  testCommon.setUp.apply(null, arguments)
  db = leveldown(testCommon.location())
  db.open(t.end.bind(t))
})

makePutErrorTest('null key', null, 'foo', /Key argument cannot be `null` or `undefined`/)
makePutErrorTest('undefined key', undefined, 'foo', /Key argument cannot be `null` or `undefined`/)
makePutErrorTest('empty String key', '', 'foo', /Key argument cannot be an empty String/)
makePutErrorTest('empty Buffer key', new Buffer(0), 'foo', /Key argument cannot be an empty Buffer/)
makePutErrorTest('empty Array key', [], 'foo', /Key argument cannot be an empty String/)

test('put() with other falsey keys works', function (t) {
  t.plan(9)

  var done = function () {
        if (++done.count == done.expected)
          return t.end()
      }

    , testPut = function (key, value) {
        done.expected++
        makePutTest(t, key, value, done)
      }

  testPut(false, 'foo false')
  testPut(0, 'foo 0')
  testPut(NaN, 'foo NaN')
})

test('put() with plain String key works', function (t) {
  makePutTest(
      t
    , 'some long string that I\'m using as a key for this unit test, cross your fingers dude, we\'re going in!'
    , 'foo'
    , t.end.bind(t)
  )
})

test('put() with plain Buffer key works', function (t) {
  fs.readFile(path.join(__dirname, 'data/testdata.bin'), function (err, data) {
    t.notOk(err, 'read test data file')
    t.type(data, Buffer)

    makePutTest(
        t
      , data
      , 'foo'
      , t.end.bind(t)
    )
  })
})

makePutErrorTest('null value', 'foo', null, /Value argument cannot be `null` or `undefined`/)
makePutErrorTest('undefined value', 'foo', undefined, /Value argument cannot be `null` or `undefined`/)
makePutErrorTest('empty String value', 'foo', '', /Value argument cannot be an empty String/)
makePutErrorTest('empty Buffer value', 'foo', new Buffer(0), /Value argument cannot be an empty Buffer/)
makePutErrorTest('empty Array value', 'foo', [], /Value argument cannot be an empty String/)

test('put() with other falsey values works', function (t) {
  t.plan(9)

  var done = function () {
        if (++done.count == done.expected)
          return t.end()
      }

    , testPut = function (key, value) {
        done.expected++
        makePutTest(t, key, value, done)
      }

  testPut('foo false', false)
  testPut('foo 0', 0)
  testPut('foo NaN', NaN)
})

test('put() with plain String value works', function (t) {
  makePutTest(
      t
    , 'foo'
    , 'some long string that I\'m using as a value for this unit test, cross your fingers dude, we\'re going in!'
    , t.end.bind(t)
  )
})

test('put() with plain Buffer value works', function (t) {
  fs.readFile(path.join(__dirname, 'data/testdata.bin'), function (err, data) {
    t.notOk(err, 'read test data file')
    t.type(data, Buffer)

    makePutTest(
        t
      , 'foo'
      , data
      , t.end.bind(t)
    )
  })
})

test('tearDown', function (t) {
  testCommon.tearDown.apply(null, arguments)
  db.close(t.end.bind(t))
})
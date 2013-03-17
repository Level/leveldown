const fs         = require('fs')
    , path       = require('path')
    , test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')

    , testBuffer = fs.readFileSync(path.join(__dirname, 'data/testdata.bin'))

/**** SETUP & UTILITY STUFF ****/

var db

  , makeGetDelErrorTests = function (type, key, expectedError) {
      test('test get() with ' + type + ' causes error', function (t) {
        db.get(key, function (err) {
          t.ok(err, 'has error')
          t.type(err, Error)
          t.like(err.message, expectedError, 'correct error message')
          t.end()
        })
      })

      test('test del() with ' + type + ' causes error', function (t) {
        db.del(key, function (err) {
          t.ok(err, 'has error')
          t.type(err, Error)
          t.like(err.message, expectedError, 'correct error message')
          t.end()
        })
      })
    }

  , makePutErrorTest = function (type, key, value, expectedError) {
      test('test put() with ' + type + ' causes error', function (t) {
        db.put(key, value, function (err) {
          t.ok(err, 'has error')
          t.type(err, Error)
          t.like(err.message, expectedError, 'correct error message')
          t.end()
        })
      })
    }

  , makePutGetDelSuccessfulTest = function (type, key, value) {
      test('test put()/get()/del() with ' + type, function (t) {
        db.put(key, value, function (err) {
          t.notOk(err, 'no error')
          db.get(key, function (err, _value) {
            t.notOk(err, 'no error, has key/value for `' + key + '`')
            t.equals(_value.toString(), value.toString())
            db.del(key, function (err) {
              t.notOk(err, 'no error, deleted key/value for `' + key + '`')
              db.get(key, function (err) {
                t.ok(err, 'entry propertly deleted')
                t.like(err.message, /NotFound/)
                t.end()
              })
            })
          })
        })
      })
    }

  , makeErrorKeyTest = function (type, key, expectedError) {
      makeGetDelErrorTests(type, key, expectedError)
      makePutErrorTest(type, key, 'foo', expectedError)
    }

/**** SETUP ENVIRONMENT ****/

test('setUp common', testCommon.setUp)
test('setUp db', function (t) {
  db = leveldown(testCommon.location())
  db.open(t.end.bind(t))
})

/**** TEST ERROR KEYS ****/

makeErrorKeyTest('null key', null, /key cannot be `null` or `undefined`/)
makeErrorKeyTest('undefined key', undefined, /key cannot be `null` or `undefined`/)
makeErrorKeyTest('empty String key', '', /key cannot be an empty String/)
makeErrorKeyTest('empty Buffer key', new Buffer(0), /key cannot be an empty Buffer/)
makeErrorKeyTest('empty Array key', [], /key cannot be an empty String/)

/**** TEST NON-ERROR KEYS ****/

// valid falsey keys
makePutGetDelSuccessfulTest('`false` key', false, 'foo false')
makePutGetDelSuccessfulTest('`0` key', 0, 'foo 0')
makePutGetDelSuccessfulTest('`NaN` key', NaN, 'foo NaN')

// standard String key
makePutGetDelSuccessfulTest(
    'long String key'
  , 'some long string that I\'m using as a key for this unit test, cross your fingers dude, we\'re going in!'
  , 'foo'
)

// Buffer key
makePutGetDelSuccessfulTest('Buffer key', testBuffer, 'foo')

// non-empty Array as a key
makePutGetDelSuccessfulTest('Array value', 'foo', [1,2,3,4])

/**** TEST ERROR VALUES ****/

makePutErrorTest('null value', 'foo', null, /value cannot be `null` or `undefined`/)
makePutErrorTest('undefined value', 'foo', undefined, /value cannot be `null` or `undefined`/)
makePutErrorTest('empty String value', 'foo', '', /value cannot be an empty String/)
makePutErrorTest('empty Buffer value', 'foo', new Buffer(0), /value cannot be an empty Buffer/)
makePutErrorTest('empty Array value', 'foo', [], /value cannot be an empty String/)

// valid falsey values
makePutGetDelSuccessfulTest('`false` value', 'foo false', false)
makePutGetDelSuccessfulTest('`0` value', 'foo 0', 0)
makePutGetDelSuccessfulTest('`NaN` value', 'foo NaN', NaN)

// standard String value
makePutGetDelSuccessfulTest(
    'long String value'
  , 'foo'
  , 'some long string that I\'m using as a key for this unit test, cross your fingers dude, we\'re going in!'
)

// standard Buffer value
makePutGetDelSuccessfulTest('Buffer key', 'foo', testBuffer)

// non-empty Array as a value
makePutGetDelSuccessfulTest('Array value', [1,2,3,4], 'foo')

/**** CLEANUP ENVIRONMENT ****/

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})

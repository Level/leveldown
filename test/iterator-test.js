const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')
    , collectEntries = testCommon.collectEntries

var db
  , sourceData = (function () {
      var d = []
        , i = 0
        , k
      for (; i < 100; i++) {
        k = (i < 10 ? '0' : '') + i
        d.push({
            type  : 'put'
          , key   : k
          , value : Math.random()
        })
      }
      return d
    }())
  , transformSource = function (d) { return { key: d.key, value: d.value } }

module.exports.setUp = function (leveldown) {
  test('setUp common', testCommon.setUp)
  test('setUp db', function (t) {
    db = leveldown(testCommon.location())
    db.open(t.end.bind(t))
  })
}

module.exports.args = function () {
  test('test argument-less iterator#next() throws', function (t) {
    var iterator = db.iterator()
    t.throws(
        iterator.next.bind(iterator)
      , { name: 'Error', message: 'next() requires a callback argument' }
      , 'no-arg iterator#next() throws'
    )
    iterator.end(t.end.bind(t))
  })

  test('test argument-less iterator#end() after next() throws', function (t) {
    var iterator = db.iterator()
    iterator.next(function () {
      t.throws(
          iterator.end.bind(iterator)
        , { name: 'Error', message: 'end() requires a callback argument' }
        , 'no-arg iterator#end() throws'
      )
      iterator.end(t.end.bind(t))
    })
  })

  test('test argument-less iterator#end() throws', function (t) {
    var iterator = db.iterator()
    t.throws(
        iterator.end.bind(iterator)
      , { name: 'Error', message: 'end() requires a callback argument' }
      , 'no-arg iterator#end() throws'
    )
    iterator.end(t.end.bind(t))
  })
}

module.exports.sequence = function () {
  test('test twice iterator#end() throws', function (t) {
    var iterator = db.iterator()
    iterator.end(function (err) {
      t.notOk(err, 'no error')
      t.throws(
          iterator.end.bind(iterator, function () {})
        , { name: 'Error', message: 'end() already called on iterator' }
        , 'no-arg iterator#next() throws'
      )
      t.end()
    })
  })

  test('test iterator#next after iterator#end() throws', function (t) {
    var iterator = db.iterator()
    iterator.end(function (err) {
      t.notOk(err, 'no error')
      t.throws(
          iterator.next.bind(iterator, function () {})
        , { name: 'Error', message: 'cannot call next() after end()' }
        , 'no-arg iterator#next() after iterator#end() throws'
      )
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
    t.throws(
        iterator.next.bind(iterator, function () {})
      , { name: 'Error', message: 'cannot call next() before previous next() has completed' }
      , 'no-arg iterator#next() throws'
    )
  })
}

module.exports.iterator = function (leveldown) {
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

  /** the following tests are mirroring the same series of tests in
    * LevelUP read-stream-test.js
    */

  test('setUp #2', function (t) {
    db.close(function () {
      db = leveldown(testCommon.location())
      db.open(function () {
        db.batch(sourceData, t.end.bind(t))
      })
    })
  })

  test('test full data collection', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, sourceData.length, 'correct number of entries')
      var expected = sourceData.map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with reverse=true', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, sourceData.length, 'correct number of entries')
      var expected = sourceData.slice().reverse().map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start=0', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '00' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, sourceData.length, 'correct number of entries')
      var expected = sourceData.map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start=50', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '50' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 50, 'correct number of entries')
      var expected = sourceData.slice(50).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start=50 and reverse=true', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '50', reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 51, 'correct number of entries')
      var expected = sourceData.slice().reverse().slice(49).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start being a midway key (49.5)', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '49.5' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 50, 'correct number of entries')
      var expected = sourceData.slice(50).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start being a midway key (49999)', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '49999' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 50, 'correct number of entries')
      var expected = sourceData.slice(50).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start being a midway key and reverse=true', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '49.5', reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 50, 'correct number of entries')
      var expected = sourceData.slice().reverse().slice(50).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with end=50', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, end: '50' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 51, 'correct number of entries')
      var expected = sourceData.slice(0, 51).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with end being a midway key (50.5)', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, end: '50.5' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 51, 'correct number of entries')
      var expected = sourceData.slice(0, 51).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with end being a midway key (50555)', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, end: '50555' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 51, 'correct number of entries')
      var expected = sourceData.slice(0, 51).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with end being a midway key and reverse=true', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, end: '50.5', reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 49, 'correct number of entries')
      var expected = sourceData.slice().reverse().slice(0, 49).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  // end='0', starting key is actually '00' so it should avoid it
  test('test iterator with end=0', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, end: '0' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 0, 'correct number of entries')
      t.end()
    })
  })

  test('test iterator with start=30 and end=70', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '30', end: '70' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 41, 'correct number of entries')
      var expected = sourceData.slice(30, 71).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start=30 and end=70 and reverse=true', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '70', end: '30', reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 41, 'correct number of entries')
      var expected = sourceData.slice().reverse().slice(29, 70).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with limit=20', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, limit: 20 }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 20, 'correct number of entries')
      var expected = sourceData.slice(0, 20).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with limit=20 and start=20', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '20', limit: 20 }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 20, 'correct number of entries')
      var expected = sourceData.slice(20, 40).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with limit=20 and reverse=true', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, limit: 20, reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 20, 'correct number of entries')
      var expected = sourceData.slice().reverse().slice(0, 20).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with limit=20 and start=20 and reverse=true', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '79', limit: 20, reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 20, 'correct number of entries')
      var expected = sourceData.slice().reverse().slice(20, 40).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with end after limit', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, limit: 20, end: '50' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 20, 'correct number of entries')
      var expected = sourceData.slice(0, 20).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with end before limit', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, limit: 50, end: '19' }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 20, 'correct number of entries')
      var expected = sourceData.slice(0, 20).map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start after database end and reverse=true', function (t) {
    collectEntries(db.iterator({ keyAsBuffer: false, valueAsBuffer: false, start: '9a', reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, sourceData.length, 'correct number of entries')
      var expected = sourceData.slice().reverse().map(transformSource)
      t.deepEqual(data, expected)
      t.end()
    })
  })

  test('test iterator with start and end after database and and reverse=true', function (t) {
    collectEntries(db.iterator({ start: '9b', end: '9a', reverse: true }), function (err, data) {
      t.notOk(err, 'no error')
      t.equal(data.length, 0, 'correct number of entries')
      t.end()
    })
  })
}

module.exports.tearDown = function () {
  test('tearDown', function (t) {
    db.close(testCommon.tearDown.bind(null, t))
  })
}

module.exports.all = function (leveldown) {
  module.exports.setUp(leveldown)
  module.exports.args()
  module.exports.sequence()
  module.exports.iterator(leveldown)
  module.exports.tearDown()
}

if (require.main === module)
  module.exports.all(leveldown)
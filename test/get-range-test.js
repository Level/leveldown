const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')

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
  , transformSource = function (d) {
      return { key: d.key, value: String(d.value) }
    }

test('setUp common', testCommon.setUp)

test('setUp db', function (t) {
  db = leveldown(testCommon.location())

  db.open(function () {
    db.batch(sourceData, t.end.bind(t))
  })
})

test('test argument-less getRange() throws', function (t) {
  t.throws(
      db.getRange.bind(db)
    , { name: 'Error', message: 'getRange() requires a callback argument' }
    , 'no-arg iterator#getRange() throws'
  )
  iterator.end(t.end.bind(t))
})

test('test full data collection', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, sourceData.length, 'correct number of entries')
    var expected = sourceData.map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with reverse=true', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, sourceData.length, 'correct number of entries')
    var expected = sourceData.slice().reverse().map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start=0', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '00' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, sourceData.length, 'correct number of entries')
    var expected = sourceData.map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start=50', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '50' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 50, 'correct number of entries')
    var expected = sourceData.slice(50).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start=50 and reverse=true', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '50', reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 51, 'correct number of entries')
    var expected = sourceData.slice().reverse().slice(49).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start being a midway key (49.5)', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '49.5' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 50, 'correct number of entries')
    var expected = sourceData.slice(50).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start being a midway key (49999)', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '49999' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 50, 'correct number of entries')
    var expected = sourceData.slice(50).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start being a midway key and reverse=true', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '49.5', reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 50, 'correct number of entries')
    var expected = sourceData.slice().reverse().slice(50).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with end=50', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, end: '50' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 51, 'correct number of entries')
    var expected = sourceData.slice(0, 51).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with end being a midway key (50.5)', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, end: '50.5' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 51, 'correct number of entries')
    var expected = sourceData.slice(0, 51).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with end being a midway key (50555)', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, end: '50555' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 51, 'correct number of entries')
    var expected = sourceData.slice(0, 51).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with end being a midway key and reverse=true', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, end: '50.5', reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 49, 'correct number of entries')
    var expected = sourceData.slice().reverse().slice(0, 49).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

// end='0', starting key is actually '00' so it should avoid it
test('test iterator with end=0', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, end: '0' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 0, 'correct number of entries')
    t.end()
  })
})

test('test iterator with start=30 and end=70', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '30', end: '70' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 41, 'correct number of entries')
    var expected = sourceData.slice(30, 71).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start=30 and end=70 and reverse=true', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '70', end: '30', reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 41, 'correct number of entries')
    var expected = sourceData.slice().reverse().slice(29, 70).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with limit=20', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, limit: 20 }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 20, 'correct number of entries')
    var expected = sourceData.slice(0, 20).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with limit=20 and start=20', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '20', limit: 20 }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 20, 'correct number of entries')
    var expected = sourceData.slice(20, 40).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with limit=20 and reverse=true', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, limit: 20, reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 20, 'correct number of entries')
    var expected = sourceData.slice().reverse().slice(0, 20).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with limit=20 and start=20 and reverse=true', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '79', limit: 20, reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 20, 'correct number of entries')
    var expected = sourceData.slice().reverse().slice(20, 40).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

// the default limit value from levelup is -1
test('test iterator with limit=-1 should iterate over whole database', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, limit: -1}, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, sourceData.length, 'correct number of entries')
    var expected = sourceData.map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with end after limit', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, limit: 20, end: '50' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 20, 'correct number of entries')
    var expected = sourceData.slice(0, 20).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with end before limit', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, limit: 50, end: '19' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 20, 'correct number of entries')
    var expected = sourceData.slice(0, 20).map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start after database end', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '9a' }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 0, 'correct number of entries')
    t.end()
  })
})

test('test iterator with start after database end and reverse=true', function (t) {
  db.getRange({ keyAsBuffer: false, valueAsBuffer: false, start: '9a', reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, sourceData.length, 'correct number of entries')
    var expected = sourceData.slice().reverse().map(transformSource)
    t.deepEqual(data, expected)
    t.end()
  })
})

test('test iterator with start and end after database and and reverse=true', function (t) {
  db.getRange({ start: '9b', end: '9a', reverse: true }, function (err, data) {
    t.notOk(err, 'no error')
    t.equal(data.length, 0, 'correct number of entries')
    t.end()
  })
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})
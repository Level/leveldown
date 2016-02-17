const test       = require('tape')
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , abstract   = require('abstract-leveldown/abstract/iterator-test')
    , make       = require('./make')
    , iota       = require('iota-array')
    , lexi       = require('lexicographic-integer')
    , util       = require('util')
    , crypto     = require('crypto')

abstract.all(leveldown, test, testCommon)

make('iterator seek throws if key is not a string or buffer', function (db, t, done) {
  var keys = [undefined, 10, true, null]
  var pending = keys.length

  keys.forEach(function (key) {
    var error
    var ite = db.iterator()

    try {
      ite.seek(key)
    } catch (e) {
      error = e
    }

    t.ok(error, 'had error from seek()')
    ite.end(end)
  })

  function end(err) {
    t.error(err, 'no error from end()')
    if (!--pending) done()
  }
})

make('iterator is seekable', function (db, t, done) {
  var ite = db.iterator()
  ite.seek('two')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key.toString(), 'two', 'key matches')
    t.same(value.toString(), '2', 'value matches')
    ite.next(function (err, key, value) {
      t.error(err, 'no error')
      t.same(key, undefined, 'end of iterator')
      t.same(value, undefined, 'end of iterator')
      ite.end(done)
    })
  })
})

make('reverse seek in the middle', function (db, t, done) {
  var ite = db.iterator({reverse: true, limit: 1})
  ite.seek('three!')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key.toString(), 'three', 'key matches')
    t.same(value.toString(), '3', 'value matches')
    ite.end(done)
  })
})

make('iterator invalid seek', function (db, t, done) {
  var ite = db.iterator()
  ite.seek('zzz')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key, undefined, 'end of iterator')
    t.same(value, undefined, 'end of iterator')
    ite.end(done)
  })
})

make('reverse seek from invalid range', function (db, t, done) {
  var ite = db.iterator({reverse: true})
  ite.seek('zzz')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key.toString(), 'two', 'end of iterator')
    t.same(value.toString(), '2', 'end of iterator')
    ite.end(done)
  })
})

make('iterator is seekable with buffer', function (db, t, done) {
  var ite = db.iterator()
  ite.seek(Buffer('two'))
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    t.equal(key.toString(), 'two', 'key matches')
    t.equal(value.toString(), '2', 'value matches')
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.equal(key, undefined, 'end of iterator')
      t.equal(value, undefined, 'end of iterator')
      ite.end(done)
    })
  })
})

make('iterator seek resets state', function (db, t, done) {
  var ite = db.iterator()
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    t.equal(key.toString(), 'one', 'key matches')
    t.ok(ite.cache, 'has cached items')
    t.equal(ite.finished, true, 'finished')
    ite.seek('two')
    t.notOk(ite.cache, 'cache is removed')
    t.equal(ite.finished, false, 'resets finished state')
    ite.end(done)
  })
})

make('iterator seek lands on or after target', function (db, t, done) {
  db.batch(pairs(10, { not: 7 }), function (err) {
    t.error(err, 'no error from batch()')

    var ite = db.iterator({gte: '4'})
    ite.seek('5')
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.equal(key.toString(), '5', 'key matches')
      t.equal(value.toString(), '5', 'value matches')
      ite.seek('7')
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')
        t.equal(key.toString(), '8', 'key matches')
        t.equal(value.toString(), '8', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('iterator reverse seek lands on or before target', function (db, t, done) {
  db.batch(pairs(10, { not: 5 }), function (err) {
    t.error(err, 'no error from batch()')

    var ite = db.iterator({reverse: true, lte: '8'})
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.equal(key.toString(), '8', 'key matches')
      t.equal(value.toString(), '8', 'value matches')
      ite.seek('5')
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')
        t.equal(key.toString(), '4', 'key matches')
        t.equal(value.toString(), '4', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('iterator seeks land on or after target', function (db, t, done) {
  var max = 100, step = 15
  db.batch(pairs(max, {lex: true, not: even}), function (err) {
    t.error(err, 'no error from batch()')

    var pending = 2
    var cb = function (err) {
      t.error(err, 'no error from end()')
      if (--pending === 0) done()
    }

    loop(db.iterator(), 0, cb)
    loop(db.iterator({gte: '0a'}), 10, cb)

    function loop(ite, i, done) {
      if (i >= max) return ite.end(done)
      ite.seek(lexi.pack(i, 'hex'))
      ite.next(function (err, key, value) {
        var expected = even(i) ? i+1 : i
        t.equal(value.toString(), '' + expected, 'k' + expected)
        loop(ite, i+step, done)
      })
    }
  })
})

make('iterator reverse seeks land on or before target', function (db, t, done) {
  var max = 100, step = 15
  db.batch(pairs(max, {lex: true, not: even}), function (err) {
    t.error(err, 'no error from batch()')

    var pending = 2
    var cb = function (err) {
      t.error(err, 'no error from end()')
      if (--pending === 0) done()
    }

    loop(db.iterator({reverse:true}), max, cb)
    loop(db.iterator({reverse:true, lte: lexi.pack(90, 'hex')}), 90, cb)

    function loop(ite, i, done) {
      if (i <= 0) return ite.end(done)
      ite.seek(lexi.pack(i, 'hex'))
      ite.next(function (err, key, value) {
        var expected = even(i) ? i-1 : i
        t.equal(value.toString(), '' + expected, 'k' + expected)
        loop(ite, i-step, done)
      })
    }
  })
})

make('iterator seek respects range', function (db, t, done) {
  db.batch(pairs(10), function (err) {
    t.error(err, 'no error from batch()')

    var pending = 0

    expect({ gt: '5' }, '4', undefined)
    expect({ gt: '5' }, '5', undefined)
    expect({ gt: '5' }, '6', '6')

    expect({ gte: '5' }, '4', undefined)
    expect({ gte: '5' }, '5', '5')
    expect({ gte: '5' }, '6', '6')

    expect({ start: '5' }, '4', undefined)
    expect({ start: '5' }, '5', '5')
    expect({ start: '5' }, '6', '6')

    expect({ lt: '5' }, '4', '4')
    expect({ lt: '5' }, '5', undefined)
    expect({ lt: '5' }, '6', undefined)

    expect({ lte: '5' }, '4', '4')
    expect({ lte: '5' }, '5', '5')
    expect({ lte: '5' }, '6', undefined)

    expect({ end: '5' }, '4', '4')
    expect({ end: '5' }, '5', '5')
    expect({ end: '5' }, '6', undefined)

    expect({ lt: '5', reverse: true }, '4', '4')
    expect({ lt: '5', reverse: true }, '5', undefined)
    expect({ lt: '5', reverse: true }, '6', undefined)

    expect({ lte: '5', reverse: true }, '4', '4')
    expect({ lte: '5', reverse: true }, '5', '5')
    expect({ lte: '5', reverse: true }, '6', undefined)

    expect({ start: '5', reverse: true }, '4', '4')
    expect({ start: '5', reverse: true }, '5', '5')
    expect({ start: '5', reverse: true }, '6', undefined)

    expect({ gt: '5', reverse: true }, '4', undefined)
    expect({ gt: '5', reverse: true }, '5', undefined)
    expect({ gt: '5', reverse: true }, '6', '6')

    expect({ gte: '5', reverse: true }, '4', undefined)
    expect({ gte: '5', reverse: true }, '5', '5')
    expect({ gte: '5', reverse: true }, '6', '6')

    expect({ end: '5', reverse: true }, '4', undefined)
    expect({ end: '5', reverse: true }, '5', '5')
    expect({ end: '5', reverse: true }, '6', '6')

    expect({ gt: '7', lt:'8' }, '7', undefined)
    expect({ gte: '7', lt:'8' }, '7', '7')
    expect({ gte: '7', lt:'8' }, '8', undefined)
    expect({ gt: '7', lte:'8' }, '8', '8')

    function expect (range, target, expected) {
      pending++
      var ite = db.iterator(range)

      ite.seek(target)
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')

        var tpl = 'seek(%s) on %s yields %s'
        var msg = util.format(tpl, target, util.inspect(range), expected)

        if (expected === undefined)
          t.equal(value, undefined, msg)
        else
          t.equal(value.toString(), expected, msg)

        ite.end(function (err) {
          t.error(err, 'no error from end()')
          if (!--pending) done()
        })
      })
    }
  })
})

make('iterator seek memory release', function (db, t, done) {
  var ite = db.iterator()
  var rssBase = process.memoryUsage().rss

  for(var i = 0; i < 1e4; i++) {
    ite.seek(crypto.randomBytes(1024).toString('hex'))

    if (i % 1000 === 0) {
      if (typeof gc != 'undefined')
        gc()
      console.log('i = %d, rss = %s %s'
        , i
        , Math.round(process.memoryUsage().rss / rssBase * 100) + '%'
        , Math.round(process.memoryUsage().rss / 1024 / 1024) + 'M'
      )
    }
  }

  ite.end(done)
})

function pairs (length, opts) {
  opts = opts || {}
  return iota(length).filter(not(opts.not)).map(function (k) {
    var key = opts.lex ? lexi.pack(k, 'hex') : '' + k
    return { type: 'put', key: key, value: '' + k }
  })
}

function not (n) {
  if (typeof n === 'function') return function (k) { return !n(k) }
  return function (k) { return k !== n }
}

function even (n) {
  return n % 2 === 0
}

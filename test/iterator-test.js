const test       = require('tape')
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , abstract   = require('abstract-leveldown/abstract/iterator-test')
    , make       = require('./make')
    , iota       = require('iota-array')
    , lexi       = require('lexicographic-integer')
    , util       = require('util')

abstract.all(leveldown, test, testCommon)

make('iterator seek throws if key is not a string or buffer', function (db, t, done) {
  var keys = [void 0, 10, true, null]
  var pending = keys.length

  keys.forEach(function(key){
    var ite = db.iterator(), error;
    try { ite.seek(key) } catch (e) { error = e }
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
    t.error(err, 'no error from next()')
    t.same(key.toString(), 'two', 'key matches')
    t.same(value.toString(), '2', 'value matches')
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.same(key, undefined, 'end of iterator')
      t.same(value, undefined, 'end of iterator')
      ite.end(done)
    })
  })
})

make('iterator is seekable with buffer', function (db, t, done) {
  var ite = db.iterator()
  ite.seek(new Buffer('two'))
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    t.same(key.toString(), 'two', 'key matches')
    t.same(value.toString(), '2', 'value matches')
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.same(key, undefined, 'end of iterator')
      t.same(value, undefined, 'end of iterator')
      ite.end(done)
    })
  })
})

make('iterator reverse seek in the middle', function (db, t, done) {
  var ite = db.iterator({reverse: true, limit: 1})
  ite.seek('three!')
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    t.same(key.toString(), 'three', 'key matches')
    t.same(value.toString(), '3', 'value matches')
    ite.end(done)
  })
})

make('iterator seek resets state', function(db, t, done) {
  var ite = db.iterator()
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    t.same(key.toString(), 'one', 'key matches')
    t.ok(ite.cache, 'has cached items')
    t.is(ite.finished, true, 'finished')
    ite.seek('two')
    t.notOk(ite.cache, 'cache is removed')
    t.is(ite.finished, false, 'resets finished state')
    done()
  })
})

make('iterator seeks', function (db, t, done) {
  db.batch(pairs(10, { not: 7 }), function(err){
    t.error(err, 'no error from batch()')

    var ite = db.iterator({gte: '4'})
    ite.seek('5')
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.same(key.toString(), '5', 'key matches')
      t.same(value.toString(), '5', 'value matches')
      ite.seek('7')
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')
        t.same(key && key.toString(), '8', 'key matches')
        t.same(value && value.toString(), '8', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('iterator seeks without cache', function (db, t, done) {
  db.batch(pairs(10, { not: 7 }), function(err){
    t.error(err, 'no error from batch()')

    var ite = db.iterator({gte: '4', highWaterMark: 1})
    ite.seek('5')
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.is(ite.cache.length, 0, 'nothing cached')
      t.same(key.toString(), '5', 'key matches')
      t.same(value.toString(), '5', 'value matches')
      ite.seek('7')
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')
        t.is(ite.cache.length, 0, 'nothing cached')
        t.same(key && key.toString(), '8', 'key matches')
        t.same(value && value.toString(), '8', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('iterator reverse seeks', function (db, t, done) {
  db.batch(pairs(10, { not: 5 }), function(err){
    t.error(err, 'no error from batch()')

    var ite = db.iterator({reverse: true, lte: '8'})
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.same(key.toString(), '8', 'key matches')
      t.same(value.toString(), '8', 'value matches')
      ite.seek('5')
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')
        t.same(key.toString(), '4', 'key matches')
        t.same(value.toString(), '4', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('iterator reverse seeks without cache', function (db, t, done) {
  db.batch(pairs(10, { not: 5 }), function(err){
    t.error(err, 'no error from batch()')

    var ite = db.iterator({reverse: true, lte: '8', highWaterMark: 1})
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.is(ite.cache.length, 0, 'nothing cached')
      t.same(key.toString(), '8', 'key matches')
      t.same(value.toString(), '8', 'value matches')
      ite.seek('5')
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')
        t.is(ite.cache.length, 0, 'nothing cached')
        t.same(key.toString(), '4', 'key matches')
        t.same(value.toString(), '4', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('iterator invalid seek', function (db, t, done) {
  var ite = db.iterator()
  ite.seek('zzz')
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    t.same(key, undefined, 'end of iterator')
    t.same(value, undefined, 'end of iterator')
    ite.end(done)
  })
})

make('iterator reverse seek from invalid range', function (db, t, done) {
  var ite = db.iterator({reverse: true})
  ite.seek('zzz')
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    t.same(key.toString(), 'two', 'end of iterator')
    t.same(value.toString(), '2', 'end of iterator')
    ite.end(done)
  })
})

make('iterator seek lands on or after target', function(db, t, done) {
  var max = 100, step = 15
  db.batch(pairs(max, {lex: true, not: even}), function(err){
    t.error(err, 'no error from batch()')

    var pending = 3
    var cb = function(err) {
      t.error(err, 'no error from end()')
      if (--pending === 0) done()
    }

    loop(db.iterator(), 0, cb)
    loop(db.iterator({gte: '0a'}), 10, cb)
    loop(db.iterator({highWaterMark: 1}), 0, cb)

    function loop(ite, i, done) {
      if (i >= max) return ite.end(done)
      ite.seek(lexi.pack(i, 'hex'))
      ite.next(function (err, key, value) {
        var expected = even(i) ? i+1 : i
        t.is(value && value.toString(), ''+expected, 'k'+expected)
        loop(ite, i+step, done)
      })
    }
  })
})

make('iterator reverse seek lands on or before target', function(db, t, done) {
  var max = 100, step = 15
  db.batch(pairs(max, {lex: true, not: even}), function(err){
    t.error(err, 'no error from batch()')

    var pending = 3
    var cb = function(err) {
      t.error(err, 'no error from end()')
      if (--pending === 0) done()
    }

    loop(db.iterator({reverse:true}), max, cb)
    loop(db.iterator({reverse:true, lte: lexi.pack(90, 'hex')}), 90, cb)
    loop(db.iterator({reverse:true, highWaterMark: 1}), max, cb)

    function loop(ite, i, done) {
      if (i <= 0) return ite.end(done)
      ite.seek(lexi.pack(i, 'hex'))
      ite.next(function (err, key, value) {
        var expected = even(i) ? i-1 : i
        t.is(value && value.toString(), ''+expected, 'k'+expected)
        loop(ite, i-step, done)
      })
    }
  })
})

make('iterator seek stays in range', function(db, t, done) {
  db.batch(pairs(10), function(err){
    t.error(err, 'no error from batch()')

    var pending = 0

    expect({ gt: 5 }, 4, undefined)
    expect({ gt: 5 }, 5, undefined)
    expect({ gt: 5 }, 6, 6)

    expect({ gte: 5 }, 4, undefined)
    expect({ gte: 5 }, 5, 5)
    expect({ gte: 5 }, 6, 6)

    expect({ lt: 5 }, 4, 4)
    expect({ lt: 5 }, 5, undefined)
    expect({ lt: 5 }, 6, undefined)

    expect({ lt: 5, reverse: true }, 4, 4)
    expect({ lt: 5, reverse: true }, 5, undefined)
    expect({ lt: 5, reverse: true }, 6, undefined)

    expect({ lte: 5, reverse: true }, 4, 4)
    expect({ lte: 5, reverse: true }, 5, 5)
    expect({ lte: 5, reverse: true }, 6, undefined)

    expect({ gt: 5, reverse: true }, 4, undefined)
    expect({ gt: 5, reverse: true }, 5, undefined)
    expect({ gt: 5, reverse: true }, 6, 6)

    function expect(range, target, expected) {
      var ite1 = create(range)
      ite1.seek(''+target)
      ite1.next(function(err, key, value) {
        verify('initial seek', ite1, err, key, value)
      })

      var ite2 = create(range)
      ite2.next(function(err, key, value) {
        t.error(err, 'no error from next()')
        t.ok(key, 'has key')
        ite2.seek(''+target)
        ite2.next(function(err, key, value) {
          verify('secondary seek', ite2, err, key, value)
        })
      })

      function verify(name, ite, err, key, value) {
        t.error(err, 'no error from next()')

        var tpl = '%s(%d) on %s yields %s'
        var msg = util.format(tpl, name, target, util.inspect(range), expected)

        t.is(value ? value.toString() : ''+value, ''+expected, msg)

        ite.end(function end(err) {
          t.error(err, 'no error from end()')
          if (!--pending) done()
        })
      }
    }

    function create(range) {
      pending++
      var opts = { highWaterMark: 1, reverse: !!range.reverse }
      if ('gt' in range) opts.gt = ''+range.gt
      if ('gte' in range) opts.gte = ''+range.gte
      if ('lt' in range) opts.lt = ''+range.lt
      if ('lte' in range) opts.lte = ''+range.lte
      return db.iterator(opts)
    }
  })
})

function pairs(length, opts) {
  opts = opts || {}
  return iota(length).filter(not(opts.not)).map(function(k){
    var key = opts.lex ? lexi.pack(k, 'hex') : ''+k
    return { type: opts.type || 'put', key: key, value: ''+k }
  })
}

function not(n) {
  if (typeof n === 'function') return function(k) { return !n(k) }
  return function(k) { return k !== n }
}

function even(n) {
  return n % 2 === 0
}

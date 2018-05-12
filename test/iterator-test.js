const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/abstract/iterator-test')
const make = require('./make')
const iota = require('iota-array')
const lexi = require('lexicographic-integer')
const util = require('util')

abstract.all(leveldown, test)

make('iterator throws if key is not a string or buffer', function (db, t, done) {
  var keys = [null, undefined, 1, true, false]
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

  function end (err) {
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

make('iterator is seekable with buffer', function (db, t, done) {
  var ite = db.iterator()
  ite.seek(Buffer.from('two'))
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

make('iterator optimized for seek', function (db, t, done) {
  var batch = db.batch()
  batch.put('a', 1)
  batch.put('b', 1)
  batch.put('c', 1)
  batch.put('d', 1)
  batch.put('e', 1)
  batch.put('f', 1)
  batch.put('g', 1)
  batch.write(function (err) {
    var ite = db.iterator()
    t.error(err, 'no error from batch')
    ite.next(function (err, key, value) {
      t.error(err, 'no error from next()')
      t.equal(key.toString(), 'a', 'key matches')
      t.equal(ite.cache.length, 0, 'no cache')
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')
        t.equal(key.toString(), 'b', 'key matches')
        t.ok(ite.cache.length > 0, 'has cached items')
        ite.seek('d')
        t.notOk(ite.cache, 'cache is removed')
        ite.next(function (err, key, value) {
          t.error(err, 'no error from next()')
          t.equal(key.toString(), 'd', 'key matches')
          t.equal(ite.cache.length, 0, 'no cache')
          ite.next(function (err, key, value) {
            t.error(err, 'no error from next()')
            t.equal(key.toString(), 'e', 'key matches')
            t.ok(ite.cache.length > 0, 'has cached items')
            ite.end(done)
          })
        })
      })
    })
  })
})

make('iterator seek before next has completed', function (db, t, done) {
  var ite = db.iterator()
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    ite.end(done)
  })
  var error
  try {
    ite.seek('two')
  } catch (e) {
    error = e
  }
  t.ok(error, 'had error from seek() before next() has completed')
})

make('close db with open iterator', function (db, t, done) {
  var ite = db.iterator()
  var cnt = 0
  ite.next(function loop (err, key, value) {
    if (cnt++ === 0) {
      t.error(err, 'no error from next()')
    } else {
      t.equal(err.message, 'iterator has ended')
    }
    if (key !== undefined) { ite.next(loop) }
  })

  db.close(function (err) {
    t.error(err, 'no error from close()')
    done(false)
  })
})

make('iterator seek after end', function (db, t, done) {
  var ite = db.iterator()
  ite.next(function (err, key, value) {
    t.error(err, 'no error from next()')
    ite.end(function (err) {
      t.error(err, 'no error from end()')
      var error
      try {
        ite.seek('two')
      } catch (e) {
        error = e
      }
      t.ok(error, 'had error from seek() after end()')
      done()
    })
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

    expect({ gt: '7', lt: '8' }, '7', undefined)
    expect({ gte: '7', lt: '8' }, '7', '7')
    expect({ gte: '7', lt: '8' }, '8', undefined)
    expect({ gt: '7', lte: '8' }, '8', '8')

    function expect (range, target, expected) {
      pending++
      var ite = db.iterator(range)

      ite.seek(target)
      ite.next(function (err, key, value) {
        t.error(err, 'no error from next()')

        var tpl = 'seek(%s) on %s yields %s'
        var msg = util.format(tpl, target, util.inspect(range), expected)

        if (expected === undefined) {
          t.equal(value, undefined, msg)
        } else {
          t.equal(value.toString(), expected, msg)
        }

        ite.end(function (err) {
          t.error(err, 'no error from end()')
          if (!--pending) done()
        })
      })
    }
  })
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

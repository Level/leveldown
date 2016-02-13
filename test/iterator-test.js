const test       = require('tape')
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , abstract   = require('abstract-leveldown/abstract/iterator-test')
    , make       = require('./make')

abstract.all(leveldown, test, testCommon)

make('iterator throws if key is not a string or buffer', function (db, t, done) {
  var ite = db.iterator()
  var error
  try {
    ite.seek()
  } catch (e) {
    error = e
  }

  t.ok(error, 'had error')
  done()
})

make('iterator is seekable', function (db, t, done) {
  var ite = db.iterator()
  ite.seek('three')
  ite.next(function (err, key, value) {
    t.error(err, 'no error')
    t.same(key.toString(), 'three', 'key matches')
    t.same(value.toString(), '3', 'value matches')

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
})

make('iterator is seekable with buffer', function (db, t, done) {
  var ite = db.iterator()
  ite.seek(new Buffer('two'))
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

make('multiple seeks', function (db, t, done) {
  var ops = [0,1,2,3,4,5,6,8,9,10].reduce(function(a,k){
    return a.push({ type: 'put', key: ''+k, value: ''+k }), a
  }, [])

  db.batch(ops, function(err){
    t.error(err, 'no error')

    var ite = db.iterator({gte: '4'})
    ite.seek('5')
    ite.next(function (err, key, value) {
      t.error(err, 'no error')
      t.same(key.toString(), '5', 'key matches')
      t.same(value.toString(), '5', 'value matches')
      ite.seek('7')
      ite.next(function (err, key, value) {
        t.error(err, 'no error')
        t.same(key && key.toString(), '8', 'key matches')
        t.same(value && value.toString(), '8', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('multiple seeks without cache', function (db, t, done) {
  var ops = [0,1,2,3,4,5,6,8,9,10].reduce(function(a,k){
    return a.push({ type: 'put', key: ''+k, value: ''+k }), a
  }, [])

  db.batch(ops, function(err){
    t.error(err, 'no error')

    var ite = db.iterator({gte: '4', highWaterMark: 1})
    ite.seek('5')
    ite.next(function (err, key, value) {
      t.error(err, 'no error')
      t.is(ite.cache.length, 0, 'nothing cached')
      t.same(key.toString(), '5', 'key matches')
      t.same(value.toString(), '5', 'value matches')
      ite.seek('7')
      ite.next(function (err, key, value) {
        t.error(err, 'no error')
        t.is(ite.cache.length, 0, 'nothing cached')
        t.same(key && key.toString(), '8', 'key matches')
        t.same(value && value.toString(), '8', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('multiple reverse seeks', function (db, t, done) {
  var ops = [0,1,2,3,4,6,7,8,9,10].reduce(function(a,k){
    return a.push({ type: 'put', key: ''+k, value: ''+k }), a
  }, [])

  db.batch(ops, function(err){
    t.error(err, 'no error')

    var ite = db.iterator({reverse: true, lte: '8'})
    ite.next(function (err, key, value) {
      t.error(err, 'no error')
      t.same(key.toString(), '8', 'key matches')
      t.same(value.toString(), '8', 'value matches')
      ite.seek('5')
      ite.next(function (err, key, value) {
        t.error(err, 'no error')
        t.same(key.toString(), '4', 'key matches')
        t.same(value.toString(), '4', 'value matches')
        ite.end(done)
      })
    })
  })
})

make('multiple reverse seeks without cache', function (db, t, done) {
  var ops = [0,1,2,3,4,6,7,8,9,10].reduce(function(a,k){
    return a.push({ type: 'put', key: ''+k, value: ''+k }), a
  }, [])

  db.batch(ops, function(err){
    t.error(err, 'no error')

    var ite = db.iterator({reverse: true, lte: '8', highWaterMark: 1})
    ite.next(function (err, key, value) {
      t.error(err, 'no error')
      t.is(ite.cache.length, 0, 'nothing cached')
      t.same(key.toString(), '8', 'key matches')
      t.same(value.toString(), '8', 'value matches')
      ite.seek('5')
      ite.next(function (err, key, value) {
        t.error(err, 'no error')
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

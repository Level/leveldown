const test         = require('tape')
    , testCommon   = require('abstract-leveldown/testCommon')
    , fs           = require('fs')
    , path         = require('path')
    , mkfiletree   = require('mkfiletree')
    , readfiletree = require('readfiletree')
    , rimraf       = require('rimraf')
    , leveldown    = require('..')
    , makeTest     = require('./make')

test('test argument-less destroy() throws', function (t) {
  t.throws(
      leveldown.destroy
    , { name: 'Error', message: 'destroy() requires `location` and `callback` arguments' }
    , 'no-arg destroy() throws'
  )
  t.end()
})

test('test callback-less, 1-arg, destroy() throws', function (t) {
  t.throws(
      leveldown.destroy.bind(null, 'foo')
    , { name: 'Error', message: 'destroy() requires `location` and `callback` arguments' }
    , 'callback-less, 1-arg destroy() throws'
  )
  t.end()
})

test('test destroy non-existent directory', function (t) {
  t.plan(4)

  var location = testCommon.location()
  var parent = path.dirname(location)

  // For symmetry with the opposite test below.
  t.ok(fs.existsSync(parent), 'parent exists before')

  // Cleanup to avoid conflicts with other tests
  rimraf(location, { glob: false }, function (err) {
    t.ifError(err, 'no rimraf error')

    leveldown.destroy(location, function () {
      t.is(arguments.length, 0, 'no arguments returned on callback')

      // Assert that destroy() didn't inadvertently create the directory.
      // Or if it did, that it was at least cleaned up afterwards.
      t.notOk(fs.existsSync(location), 'directory does not exist after')
    })
  })
})

test('test destroy non-existent parent directory', function (t) {
  t.plan(3)

  var location = '/1/2/3/4'
  var parent = path.dirname(location)

  t.notOk(fs.existsSync(parent), 'parent does not exist before')

  leveldown.destroy(location, function () {
    t.is(arguments.length, 0, 'no arguments returned on callback')
    t.notOk(fs.existsSync(location), 'directory does not exist after')
  })
})

test('test destroy non leveldb directory', function (t) {
  var tree = {
      'foo': 'FOO'
    , 'bar': { 'one': 'ONE', 'two': 'TWO', 'three': 'THREE' }
  }
  mkfiletree.makeTemp('destroy-test', tree, function (err, dir) {
    t.notOk(err, 'no error')
    leveldown.destroy(dir, function () {
      t.ok(arguments, 'no arguments')
      readfiletree(dir, function (err, actual) {
        t.notOk(err, 'no error')
        t.deepEqual(actual, tree, 'directory remains untouched')
        mkfiletree.cleanUp(function (err) {
          t.notOk(err, 'no error')
          t.end()
        })
      })
    })
  })
})

makeTest('test destroy() cleans and removes leveldb-only dir', function (db, t, done, location) {
  db.close(function (err) {
    t.notOk(err, 'no error')
    leveldown.destroy(location, function () {
      t.notOk(fs.existsSync(location), 'directory completely removed')
      done(false)
    })
  })
})

makeTest('test destroy() cleans and removes only leveldb parts of a dir', function (db, t, done, location) {
  fs.writeFileSync(path.join(location, 'foo'), 'FOO')
  db.close(function (err) {
    t.notOk(err, 'no error')
    leveldown.destroy(location, function () {
      readfiletree(location, function (err, tree) {
        t.deepEqual(tree, { 'foo': 'FOO' }, 'non-leveldb files left intact')
        done(false)
      })
    })
  })
})

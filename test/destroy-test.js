const test = require('tape')
const tempy = require('tempy')
const fs = require('fs')
const path = require('path')
const mkfiletree = require('mkfiletree')
const readfiletree = require('readfiletree')
const rimraf = require('rimraf')
const leveldown = require('..')
const makeTest = require('./make')

test('test argument-less destroy() throws', function (t) {
  t.throws(leveldown.destroy, {
    name: 'Error',
    message: 'destroy() requires `location` and `callback` arguments'
  }, 'no-arg destroy() throws')
  t.end()
})

test('test callback-less, 1-arg, destroy() throws', function (t) {
  t.throws(leveldown.destroy.bind(null, 'foo'), {
    name: 'Error',
    message: 'destroy() requires `location` and `callback` arguments'
  }, 'callback-less, 1-arg destroy() throws')
  t.end()
})

test('test destroy non-existent directory', function (t) {
  t.plan(4)

  var location = tempy.directory()
  var parent = path.dirname(location)

  // For symmetry with the opposite test below.
  t.ok(fs.existsSync(parent), 'parent exists before')

  // Cleanup to avoid conflicts with other tests
  rimraf(location, { glob: false }, function (err) {
    t.ifError(err, 'no error from rimraf()')

    leveldown.destroy(location, function (err) {
      t.error(err, 'no error')

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
    foo: 'FOO',
    bar: { 'one': 'ONE', 'two': 'TWO', 'three': 'THREE' }
  }

  mkfiletree.makeTemp('destroy-test', tree, function (err, dir) {
    t.ifError(err, 'no error from makeTemp()')

    leveldown.destroy(dir, function (err) {
      t.ifError(err, 'no error from destroy()')

      readfiletree(dir, function (err, actual) {
        t.ifError(err, 'no error from readfiletree()')
        t.deepEqual(actual, tree, 'directory remains untouched')

        mkfiletree.cleanUp(function (err) {
          t.ifError(err, 'no error from cleanup()')
          t.end()
        })
      })
    })
  })
})

makeTest('test destroy() cleans and removes leveldb-only dir', function (db, t, done) {
  var location = db.location
  db.close(function (err) {
    t.ifError(err, 'no error from close()')

    leveldown.destroy(location, function (err) {
      t.ifError(err, 'no error from destroy()')
      t.notOk(fs.existsSync(location), 'directory completely removed')

      done(null, false)
    })
  })
})

makeTest('test destroy() cleans and removes only leveldb parts of a dir', function (db, t, done) {
  var location = db.location
  fs.writeFileSync(path.join(location, 'foo'), 'FOO')

  db.close(function (err) {
    t.ifError(err, 'no error from close()')

    leveldown.destroy(location, function (err) {
      t.ifError(err, 'no error from destroy()')

      readfiletree(location, function (err, tree) {
        t.ifError(err, 'no error from readfiletree()')
        t.deepEqual(tree, { 'foo': 'FOO' }, 'non-leveldb files left intact')

        done(null, false)
      })
    })
  })
})

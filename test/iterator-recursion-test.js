const test = require('tape')
const testCommon = require('./common')
const leveldown = require('..')
const fork = require('child_process').fork
const path = require('path')

let db

const sourceData = (function () {
  var d = []
  var i = 0
  var k
  for (; i < 100000; i++) {
    k = (i < 10 ? '0' : '') + i
    d.push({
      type: 'put',
      key: k,
      value: Math.random()
    })
  }
  return d
}())

test('setUp common', testCommon.setUp)

test('try to create an iterator with a blown stack', function (t) {
  // Reducing the stack size down from the default 984 for the child node
  // process makes it easier to trigger the bug condition. But making it too low
  // causes the child process to die for other reasons.
  var opts = { execArgv: [ '--stack-size=128' ] }
  var child = fork(path.join(__dirname, 'stack-blower.js'), [ 'run' ], opts)

  t.plan(2)

  child.on('message', function (m) {
    t.ok(true, m)
    child.disconnect()
  })

  child.on('exit', function (code, sig) {
    t.equal(code, 0, 'child exited normally')
  })
})

test('setUp db', function (t) {
  db = leveldown(testCommon.location())
  db.open(function (err) {
    t.error(err)
    db.batch(sourceData, t.end.bind(t))
  })
})

test('iterate over a large iterator with a large watermark', function (t) {
  var iterator = db.iterator({
    highWaterMark: 10000000
  })
  var read = function () {
    iterator.next(function () {
      if (!arguments.length) {
        t.end()
      } else {
        read()
      }
    })
  }

  read()
})

test('tearDown', function (t) {
  db.close(testCommon.tearDown.bind(null, t))
})

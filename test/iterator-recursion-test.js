const test = require('tape')
const testCommon = require('./common')
const fork = require('child_process').fork
const path = require('path')

let db

const sourceData = (function () {
  const d = []
  let i = 0
  let k
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

// TODO: fix this test. It asserted that we didn't segfault if user code had an
// infinite loop leading to stack exhaustion, which caused a node::FatalException()
// call in our Iterator to segfault. This was fixed in 2014 (commit 85e6a38).
//
// Today (2020), we see occasional failures in CI again. We no longer call
// node::FatalException() so there's a new reason. Possibly related to
// https://github.com/Level/leveldown/issues/667.
test.skip('try to create an iterator with a blown stack', function (t) {
  for (let i = 0; i < 100; i++) {
    t.test(`try to create an iterator with a blown stack (${i})`, function (t) {
      t.plan(3)

      // Reducing the stack size down from the default 984 for the child node
      // process makes it easier to trigger the bug condition. But making it too low
      // causes the child process to die for other reasons.
      const opts = { execArgv: ['--stack-size=128'] }
      const child = fork(path.join(__dirname, 'stack-blower.js'), ['run'], opts)

      child.on('message', function (m) {
        t.ok(true, m)
        child.disconnect()
      })

      child.on('exit', function (code, sig) {
        t.is(code, 0, 'child exited normally')
        t.is(sig, null, 'not terminated due to signal')
      })
    })
  }

  t.end()
})

test('setUp db', function (t) {
  db = testCommon.factory()
  db.open(function (err) {
    t.error(err)
    db.batch(sourceData, t.end.bind(t))
  })
})

test('iterate over a large iterator with a large watermark', function (t) {
  const iterator = db.iterator({
    highWaterMark: 10000000
  })
  const read = function () {
    iterator.next(function (err, key, value) {
      if (err) throw err

      if (key === undefined && value === undefined) {
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

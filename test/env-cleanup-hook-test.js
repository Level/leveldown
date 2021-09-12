'use strict'

const test = require('tape')
const fork = require('child_process').fork
const path = require('path')

// Test env_cleanup_hook at several stages of a db lifetime
addTest(['create'])
addTest(['create', 'open'])
addTest(['create', 'open', 'create-iterator'])
addTest(['create', 'open', 'create-iterator', 'close'])
addTest(['create', 'open', 'create-iterator', 'nexting'])
addTest(['create', 'open', 'create-iterator', 'nexting', 'close'])
addTest(['create', 'open', 'close'])
addTest(['create', 'open-error'])

function addTest (steps) {
  test(`cleanup on environment exit (${steps.join(', ')})`, function (t) {
    t.plan(3)

    const child = fork(path.join(__dirname, 'env-cleanup-hook.js'), steps)

    child.on('message', function (m) {
      t.is(m, steps[steps.length - 1], `got to step: ${m}`)
      child.disconnect()
    })

    child.on('exit', function (code, sig) {
      t.is(code, 0, 'child exited normally')
      t.is(sig, null, 'not terminated due to signal')
    })
  })
}

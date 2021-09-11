'use strict'

const testCommon = require('./common')

function test (steps) {
  let step

  function nextStep () {
    step = steps.shift() || step
    return step
  }

  if (nextStep() !== 'create') {
    // Send a message triggering an environment exit
    // and indicating at which step we stopped.
    return process.send(step)
  }

  const db = testCommon.factory()

  if (nextStep() !== 'open') {
    if (nextStep() === 'open-error') {
      // If opening fails the cleanup hook should be a noop.
      db.open({ createIfMissing: false, errorIfExists: true }, function (err) {
        if (!err) throw new Error('Expected an open() error')
      })
    }

    return process.send(step)
  }

  // Open the db, expected to be closed by the cleanup hook.
  db.open(function (err) {
    if (err) throw err

    if (nextStep() === 'create-iterator') {
      // Create an iterator, expected to be ended by the cleanup hook.
      const it = db.iterator()

      if (nextStep() === 'nexting') {
        // This async work should finish before the cleanup hook is called.
        it.next(function (err) {
          if (err) throw err
        })
      }
    }

    if (nextStep() === 'close') {
      // Close the db, after which the cleanup hook is a noop.
      db.close(function (err) {
        if (err) throw err
      })
    }

    process.send(step)
  })
}

test(process.argv.slice(2))

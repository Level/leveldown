const test = require('tape')
const concat = require('level-concat-iterator')
const testCommon = require('./common')
const rangeOptions = ['gt', 'gte', 'lt', 'lte']

test('empty range options are ignored', function (t) {
  const db = testCommon.factory()

  t.test('setup', function (t) {
    db.open(function (err) {
      t.ifError(err, 'no open error')

      db.batch()
        .put(Buffer.from([0]), 'value')
        .put(Buffer.from([126]), 'value')
        .write(t.end.bind(t))
    })
  })

  rangeOptions.forEach(function (option) {
    t.test(option, function (t) {
      t.plan(8)

      concat(db.iterator({ [option]: Buffer.alloc(0) }), verifyBuffer)
      concat(db.iterator({ [option]: Buffer.alloc(0), keyAsBuffer: false }), verifyString)
      concat(db.iterator({ [option]: '' }), verifyBuffer)
      concat(db.iterator({ [option]: '', keyAsBuffer: false }), verifyString)

      function verifyBuffer (err, entries) {
        t.ifError(err, 'no concat error')
        t.same(entries.map(getKey), [Buffer.from([0]), Buffer.from([126])])
      }

      function verifyString (err, entries) {
        t.ifError(err, 'no concat error')
        t.same(entries.map(getKey), ['\x00', '~'])
      }

      function getKey (entry) {
        return entry.key
      }
    })
  })

  t.test('teardown', function (t) {
    db.close(t.end.bind(t))
  })

  t.end()
})

const test = require('tape')
const leveldown = require('..')

test('test database creation non-string location throws', function (t) {
  t.throws(
    leveldown.bind(null, {}),
    /constructor requires a location string argument/,
    'non-string location leveldown() throws'
  )
  t.end()
})

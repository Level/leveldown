const test      = require('tap').test
    , leveldown = require('../')

module.exports.args = function (leveldown) {
  test('test database creation no-arg throws', function (t) {
    t.throws(
        leveldown
      , { name: 'Error', message: 'leveldown() requires at least a location argument' }
      , 'no-arg leveldown() throws'
    )
    t.end()
  })

  test('test database open no-arg throws', function (t) {
    var db = leveldown('foo')
    t.ok(db, 'database object returned')
    t.isa(db.open, 'function', 'open() function exists')
    t.end()
  })
}

if (require.main === module)
  module.exports.args(leveldown)
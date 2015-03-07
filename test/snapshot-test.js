const test       = require('tap').test
    , testCommon = require('abstract-leveldown/testCommon')
    , leveldown  = require('../')
    , makeTest   = require('./make-test')

makeTest('test snapshot', function (db, t, done) {
  db.put('x', '1', function (err) {
    var snapshot = db.snapshot();
    db.put('x', '2', function (err) {
      db.get('x', { snapshot: snapshot }, function(err, data) {
        t.equal('' + data, '1', 'should have read from snapshot');
        done();
      });
    });
  });
})

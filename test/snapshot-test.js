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

makeTest('test snapshot invalid type with internal field',
         function (db, t, done) {
  db.put('x', '1', function (err) {
    try {
      db.get('x', { snapshot: db.iterator() }, function(err, data) {
        t.notOk(true, 'should have thrown exception');
        done();
      });
    } catch (err) {
      t.ok(err);
      t.equal('Snapshot type is incorrect', err.message, 'exception message');
      done();
    }
  });
})

makeTest('test snapshot invalid type without internal field',
         function (db, t, done) {
  db.put('x', '1', function (err) {
    try {
      db.get('x', { snapshot: Object() }, function(err, data) {
        t.notOk(true, 'should have thrown exception');
        done();
      });
    } catch (err) {
      t.ok(err);
      t.equal('Snapshot type is incorrect', err.message, 'exception message');
      done();
    }
  });
})

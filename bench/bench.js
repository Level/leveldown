var Leveldown = require('../');
var assert = require('assert');

Leveldown = new Leveldown(__dirname+"/dbfoo");

  Leveldown.open({
        errorIfExists   : false
      , createIfMissing : true
      , cacheSize       : 8 << 20
      , writeBufferSize : 4 << 20
  })


// bench -----------------------------------

var count = 12000;
var val = '1337,1337,1337,1337,1337';

function putSync () {
  var start = Date.now();

  for (var i = 0; i < count; i++) {
    Leveldown.put(i, val);
  }

  var duration = Date.now()-start;
  log(true, count, 'put', duration);
}

function getSync () {
  start = Date.now();

  for (var i = 0; i < count; i++) {
    assert(Leveldown.get(i) == val);
  }

  duration = Date.now()-start;
  log(true, count, 'get', duration);
}

function putAsync(cb) {
  start = Date.now();

  var written = 0;
  for (var i = 0; i < count; i++) {
    Leveldown.put(i, val, function (err) {
      if (err) throw err;
      if (++written == count) {
        duration = Date.now()-start;
        log(false, count, 'put', duration);
        if (cb) cb();
      }
    })
  }
}

function getAsync (cb) {
  start = Date.now();

  var received = 0;
  for (var i = 0; i < count; i++) {
    Leveldown.get(i, function (err, value) {
      if (err) throw err;
      assert(value == val);
      if (++received == count) {
        duration = Date.now()-start;
        log(false, count, 'get', duration);
        if (cb) cb()
      }
    })
  }
}

function batchSync () {
  var start = Date.now();

  var arr = [];
  for (var i = 0; i < count; i++) {
    arr.push({type:"put",key:i, value:val});
  }

  var batch = Leveldown.batch(arr);

  var duration = Date.now()-start;
  log(true, count, 'batch', duration);
}

function batchAsync(cb) {
  var start = Date.now();

  var arr = [];
  for (var i = 0; i < count; i++) {
    arr.push({type:"put",key:i, value:val});
  }
  
  Leveldown.batch(arr, function (err) {
    if (err) throw err;
    var duration = Date.now() - start;
    log(false, count, 'batch', duration);
    if (cb) cb();
  });
}


/*
 * Get it started
 */

console.log('\n  benchmarking with ' + humanize(count) + ' records, ' + val.length + ' chars each\n');

putAsync(function () {
  putSync()
  batchAsync(function () {
    batchSync()
    getAsync(function () {
      getSync()
      console.log()
    })
  })
})

/*
 * Utility functions
 */

var last;
function log(sync, num, op, dur) {
  if (last && last != op) console.log();
  last = op;
  console.log([
    pad(op + (sync? 'Sync' : ''), 13),
    ':',
    pad(humanize(Math.floor(1000/dur * num)), 9),
    {'batch' : 'w', 'put' : 'w', 'get' : 'r', 'iterator' : 'r'}[op] + '/s',
    'in ' + pad(humanize(dur), 6) + 'ms'
   ].join(' '));
}

function pad(str, len) {
  str = str.toString();
  return Array(len - str.length + 1).join(' ') + str;
}

function humanize(n, options){
  options = options || {};
  var d = options.delimiter || ',';
  var s = options.separator || '.';
  n = n.toString().split('.');
  n[0] = n[0].replace(/(\d)(?=(\d\d\d)+(?!\d))/g, '$1' + d);
  return n.join(s);
}

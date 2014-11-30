var leveldown = require('../');
var assert = require('assert');

leveldown = new leveldown(__dirname+"/foodb");

try {
  leveldown.open({
        errorIfExists   : false
      , createIfMissing : true
      , cacheSize       : 8 << 20
      , writeBufferSize : 4 << 20
  });
  benchIt();
} catch(e) {
  leveldown.open({
        errorIfExists   : false
      , createIfMissing : true
      , cacheSize       : 8 << 20
      , writeBufferSize : 4 << 20
  }, function(err){
    if (err) throw err;
    benchIt();
  })
}


// bench -----------------------------------
function benchIt() {
var count = 12000;
var val = '1337,1337,1337,1337,1337';

function putSync () {
  if (!leveldown.putSync) {
    return;
  }
  var start = Date.now();

  for (var i = 0; i < count; i++) {
    leveldown.put(i, val);
  }

  var duration = Date.now()-start;
  log(true, count, 'put', duration);
}

function putSyncDirect () {
  if (!leveldown.putSync) {
    return;
  }
  var start = Date.now();

  for (var i = 0; i < count; i++) {
    leveldown.putSync(i, val);
  }

  var duration = Date.now()-start;
  log(true, count, 'put', duration, 'directly');
}

function getSync () {
  if (!leveldown.getSync) {
    return;
  }
  start = Date.now();

  for (var i = 0; i < count; i++) {
    assert(leveldown.get(i) == val);
  }

  duration = Date.now()-start;
  log(true, count, 'get', duration,"asBuffer");
}

function getSyncDirect () {
  if (!leveldown.getSync) {
    return;
  }
  start = Date.now();

  for (var i = 0; i < count; i++) {
    assert(leveldown.getSync(i) == val);
  }

  duration = Date.now()-start;
  log(true, count, 'get', duration, 'directly');
}

function getStrSync () {
  if (!leveldown.getSync) {
    return;
  }
  start = Date.now();

  for (var i = 0; i < count; i++) {
    assert(leveldown.get(i, {asBuffer: false}) == val);
  }

  duration = Date.now()-start;
  log(true, count, 'get', duration);
}

function putAsync(cb) {
  start = Date.now();

  var written = 0;
  for (var i = 0; i < count; i++) {
    leveldown.put(i, val, function (err) {
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
    leveldown.get(i, function (err, value) {
      if (err) throw err;
      assert(value == val);
      if (++received == count) {
        duration = Date.now()-start;
        log(false, count, 'get', duration, "asBuffer");
        if (cb) cb()
      }
    })
  }
}

function getStrAsync (cb) {
  start = Date.now();

  var received = 0;
  for (var i = 0; i < count; i++) {
    leveldown.get(i, {asBuffer:false}, function (err, value) {
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
  if (!leveldown.batchSync) {
    return;
  }
  var start = Date.now();

  var arr = [];
  for (var i = 0; i < count; i++) {
    arr.push({type:"put",key:i, value:val});
  }

  var batch = leveldown.batch(arr);

  var duration = Date.now()-start;
  log(true, count, 'batch', duration);
}

function batchSyncDirect () {
  if (!leveldown.batchSync) {
    return;
  }
  var start = Date.now();

  var arr = [];
  for (var i = 0; i < count; i++) {
    arr.push({type:"put",key:i, value:val});
  }

  var batch = leveldown.batchSync(arr);

  var duration = Date.now()-start;
  log(true, count, 'batch', duration, 'directly');
}

function batchAsync(cb) {
  var start = Date.now();

  var arr = [];
  for (var i = 0; i < count; i++) {
    arr.push({type:"put",key:i, value:val});
  }
  
  leveldown.batch(arr, function (err) {
    if (err) throw err;
    var duration = Date.now() - start;
    log(false, count, 'batch', duration);
    if (cb) cb();
  });
}

function iteratorAsync (cb) {
  start = Date.now();

  var received = 0;
  var result=[];
  var iterator = leveldown.iterator()
    , fn = function (err, key, value) {
        if (err) {
          console.log(err)
        }
        else if (key && value) {
          received++
          result.push(key)
          result.push(value)
          process.nextTick(next)
        } else { // end
          iterator.end(function () {
            var duration = Date.now()-start;
            assert(received == count);
            log(false, received, 'iterator', duration, "asBuffer");
            if (cb) cb()
          })
        }
      }
    , next = function () {
        iterator.next(fn)
      }
    next();
}

function iteratorStrAsync (cb) {
  start = Date.now();

  var received = 0;
  var result=[];
  var iterator = leveldown.iterator({valueAsBuffer: false, keyAsBuffer: false})
    , fn = function (err, key, value) {
        if (err) {
          console.log(err)
        }
        else if (key && value) {
          received++
          result.push(key)
          result.push(value)
          process.nextTick(next)
        } else { // end
          iterator.end(function () {
            var duration = Date.now()-start;
            assert(received == count);
            log(false, received, 'iterator', duration);
            if (cb) cb()
          })
        }
      }
    , next = function () {
        iterator.next(fn)
      }
    next();
}

function iteratorStrAsyncDirect (cb) {
  start = Date.now();

  var received = 0;
  var result=[];
  var iterator = leveldown.iterator({keyAsBuffer: false, valueAsBuffer:false, highWaterMark: 16*1024*8}) //22 will get all at once.
    , fn = function (err, arr, finished) {
        if (err) {
          console.log(err)
          if (cb) cb()
          return
        }
        received += arr.length / 2
        result = result.concat(arr)
        if (!finished) {
          process.nextTick(next)
        } else { // end
          iterator.end(function () {
            var duration = Date.now()-start;
            log(false, received, 'iterator', duration, "directly");
            if (cb) cb()
          })
        }
      }
    , next = function () {
        iterator.binding.next(fn)
      }
    next();
}

function iteratorStrSyncDirect () {
  if (!leveldown.putSync) {
    return;
  }
  start = Date.now();

  var received = 0;
  var iterator = leveldown.iterator({keyAsBuffer: false, valueAsBuffer:false, highWaterMark: 16*1024*8}); //, highWaterMark: 16*1024*22, 22 will get all at once.
  var result = [];
  var results = iterator.nextSync(result);
  var size = results[1];
  while (size > 0) {
    result = result.concat(results[0])
    received += size;
    //result = [];
    results = iterator.nextSync(result);
    size = results[1];
  }
  result = result.concat(results[0])
  received += Math.abs(size);
  var duration = Date.now()-start;
  assert(received == count);
  log(true, received, 'iterator', duration, "directly");
  iterator.endSync()
}


/*
 * Get it started
 */

console.log('\n  benchmarking with ' + humanize(count) + ' records, ' + val.length + ' chars each\n');

putAsync(function () {
  putSync()
  putSyncDirect()
  batchAsync(function () {
    batchSync()
    batchSyncDirect()
    getAsync(function () {
      getStrAsync(function () {
        getSync()
        getStrSync()
        getSyncDirect()
        iteratorAsync(function(){
          iteratorStrAsync(function(){
            iteratorStrAsyncDirect(function(){
              iteratorStrSyncDirect()
              console.log()
            })
          })
        })
      })
    })
  })
})

/*
 * Utility functions
 */

var last;
function log(sync, num, op, dur, desc) {
  if (!desc) desc = ""
  else desc = "("+desc+")";
  if (last && last != op) console.log();
  last = op;
  console.log([
    pad(op + (sync? 'Sync' : ''), 13),
    ':',
    pad(humanize(Math.floor(1000/dur * num)), 9),
    {'batch' : 'w', 'put' : 'w', 'get' : 'r', 'iterator' : 'r'}[op] + '/s',
    'in ' + pad(humanize(dur), 6) + 'ms' + pad(desc, 18)
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

}

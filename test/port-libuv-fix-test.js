const test = require('tap').test
    , path = require('path')
    , fs   = require('fs')

test('test port-libuv is being used', function (t) {
  var version = fs.readFileSync(path.join(__dirname, '../deps/leveldb/leveldb.gyp'), 'utf8')
                  .match(/'ldbversion': '([^']+)'/)[1]
    , porth

  t.equal(version, 'hyper', 'matched current leveldb version')

  // TODO 'deps/leveldb/leveldb-hyper/port/port.h does not include 'port_uv.h'
  // so we do not have support for windows at the moment (@ralphtheninja)

  //porth = fs.readFileSync(path.join(__dirname, '../deps/leveldb/leveldb-' + version + '/port/port.h'), 'utf8')
  //t.ok(/"port_uv\.h"/.test(porth), 'port.h includes reference to port_uv.h')

  t.end()
})

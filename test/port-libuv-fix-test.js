const test = require('tape')
const path = require('path')
const fs = require('fs')

test('test port-libuv is being used', function (t) {
  var version = fs.readFileSync(path.join(__dirname, '../deps/leveldb/leveldb.gyp'), 'utf8')
    .match(/'ldbversion': '([^']+)'/)[1]
  var porth

  t.ok(version, 'matched current leveldb version')

  porth = fs.readFileSync(path.join(__dirname, '../deps/leveldb/leveldb-' + version + '/port/port.h'), 'utf8')

  t.ok(/"port_uv\.h"/.test(porth), 'port.h includes reference to port_uv.h')

  t.end()
})

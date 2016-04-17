var leveldown = require('../')

var addr = '1111111111111111111114oLvT2'

var db = leveldown(process.env.HOME + '/iterleak.db')
var records = {
  'w/a/14r6JPSJNzBXXJEM2jnmoybQCw3arseKuY/primary': '00',
  'w/a/17nJuKqjTyAeujSJnPCebpSTEz1v9kjNKg/primary': '00',
  'w/a/18cxWLCiJMESL34Ev1LJ2meGTgL14bAxfj/primary': '00',
  'w/a/18pghEAnqCRTrjd7iyUj6XNKmSNx4fAywB/primary': '00',
  'w/a/19EWPPzY6XkQeM7DxqJ4THbY3DGekRQKbt/primary': '00',
  'w/a/1DKDeLQyBCkV5aMG15XbAdpwwHnxqw9rvY/primary': '00',
  'w/a/1HSJAoU5TaGKhAJxwpTC1imiMM1ab8SFGW/primary': '00',
  'w/a/1HkeafxVvStf2Np6wxUjkTpCt1gTDJSLpi/primary': '00',
  'w/a/1Hr5JduPFdZ4n4dHBUqRoxLQEG4P93C658/primary': '00',
  'w/a/1KATodK9Ko8MchJZzDxLhjWz4d8oAuCqEh/primary': '00',
  'w/a/1NhRKhiAAJrmwXoLhL9dGG1z6oMiFGrxZ7/primary': '00',
  'w/a/1yTq3DpyUNqUCxDttczGjufbEBKAXMTSq/primary': '00',
  'w/w/primary': '00'
}

db.open({
  createIfMissing: true,
  errorIfExists: false,
  compression: true,
  cacheSize: 8 << 20,
  writeBufferSize: 4 << 20,
  maxOpenFiles: 8192
}, function(err) {
  if (err)
    throw err

  memory()

  var batch = db.batch();
  Object.keys(records).forEach(function(key) {
    var value = new Buffer(records[key], 'hex')
    batch.put(key, value)
  })

  batch.write(function(err) {
    if (err)
      throw err

    // This will leak roughly 1mb per call.
    setTimeout(function self() {
      var i = 0
      ;(function next(err) {
        if (err)
          throw err
        if (i++ >= 10000) {
          memory()
          return setTimeout(self, 1000)
        }
        iterate(addr, next)
      })()
    }, 1000)
  })
})

function memory() {
  var mem = process.memoryUsage()
  console.log('Memory: rss=%dmb, js-heap=%d/%dmb native-heap=%dmb',
              mb(mem.rss),
              mb(mem.heapUsed),
              mb(mem.heapTotal),
              mb(mem.rss - mem.heapTotal))
}

function mb(size) {
  return size / 1024 / 1024 | 0
}

function iterate(address, callback) {
  var iter = db.iterator({
    gte: 'w/a/' + address,
    lte: 'w/a/' + address + '~',
    keys: true,
    values: false,
    fillCache: false,
    keyAsBuffer: false
  })

  ;(function next() {
    iter.next(function(err, key, value) {
      if (err) {
        return iter.end(function(e) {
          if (e)
            throw e
          callback(err)
        })
      }

      if (key === undefined)
        return iter.end(callback)

      next()
    })
  })()
}

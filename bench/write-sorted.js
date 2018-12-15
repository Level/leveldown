const leveldown = require('../')
const timestamp = require('monotonic-timestamp')
const crypto = require('crypto')
const fs = require('fs')
const du = require('du')
const path = require('path')
const rimraf = require('rimraf')
const argv = require('optimist').argv

const options = {
  db: argv.db || path.join(__dirname, 'db'),
  num: argv.num || 10000000,
  concurrency: argv.concurrency || 10,
  cacheSize: argv.cacheSize || 8,
  writeBufferSize: argv.writeBufferSize || 4,
  valueSize: argv.valueSize || 100,
  out: argv.out || path.join(__dirname, 'write-sorted.csv')
}

const data = crypto.randomBytes(256) // buffer

rimraf.sync(options.db)

const db = leveldown(options.db)
const timesStream = fs.createWriteStream(options.out, 'utf8')

function report (ms) {
  console.log('Wrote', options.num, 'in', Math.floor(ms / 1000) + 's')
  timesStream.end()
  du(options.db, function (err, size) {
    if (err) throw err
    console.log('Database size:', Math.floor(size / 1024 / 1024) + 'M')
  })
  console.log('Wrote times to ', options.out)
}

db.open(function (err) {
  if (err) throw err

  let inProgress = 0
  let totalWrites = 0
  const startTime = Date.now()
  let writeBuf = ''

  function write () {
    if (totalWrites % 100000 === 0) console.log(inProgress, totalWrites)

    if (totalWrites % 1000 === 0) {
      timesStream.write(writeBuf)
      writeBuf = ''
    }

    if (totalWrites++ === options.num) return report(Date.now() - startTime)
    if (inProgress >= options.concurrency || totalWrites > options.num) return

    var start = process.hrtime()
    inProgress++

    db.put(timestamp(), data, function (err) {
      if (err) throw err
      var duration = process.hrtime(start)
      var nano = (duration[0] * 1e9) + duration[1]

      writeBuf += (Date.now() - startTime) + ',' + nano + '\n'
      inProgress--
      process.nextTick(write)
    })

    process.nextTick(write)
  }

  write()
})

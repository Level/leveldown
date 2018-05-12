const leveldown = require('../')
const crypto = require('crypto')
const fs = require('fs')
const du = require('du')
const uuid = require('node-uuid')

const entryCount = 10000000
const concurrency = 10
const timesFile = './write_random_times.csv'
const dbDir = './write_random.db'
const data = crypto.randomBytes(256) // buffer

const db = leveldown(dbDir)
const timesStream = fs.createWriteStream(timesFile, 'utf8')

function report (ms) {
  console.log('Wrote', entryCount, 'in', Math.floor(ms / 1000) + 's')
  timesStream.end()
  du(dbDir, function (err, size) {
    if (err) throw err
    console.log('Database size:', Math.floor(size / 1024 / 1024) + 'M')
  })
  console.log('Wrote times to ', timesFile)
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

    if (totalWrites++ === entryCount) return report(Date.now() - startTime)
    if (inProgress >= concurrency || totalWrites > entryCount) return

    var time = process.hrtime()
    inProgress++

    db.put(uuid.v4(), data, function (err) {
      if (err) throw err
      writeBuf += (Date.now() - startTime) + ',' + process.hrtime(time)[1] + '\n'
      inProgress--
      process.nextTick(write)
    })

    process.nextTick(write)
  }

  write()
})

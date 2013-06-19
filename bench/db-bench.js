#!/usr/bin/env node

const leveldown = require('../')
    , fs        = require('fs')
    , du        = require('du')
    , rimraf    = require('rimraf')

    , argv      = require('optimist').argv

    , options   = {
          benchmark        : argv.benchmark
        , useExisting      : argv.use_existing
        , db               : argv.db
        , num              : argv.num             || 1000000
        , concurrency      : argv.concurrency     || 4
        , cacheSize        : argv.cacheSize       || 8
        , writeBufferSize  : argv.writeBufferSize || 4
        , valueSize        : argv.valueSize       || 100
        , timingOutput     : argv.timingOutput
        , throughputOutput : argv.throughputOutput
      }

    , randomData = require('./random-data')()
    , keyTmpl = '0000000000000000'

if (!options.useExisting) {
  leveldown.destroy(options.db, function () {})
}

var db          = leveldown(options.db, {
        errorIfExists   : false
      , createIfMissing : true
      , cacheSize       : options.cacheSize << 20
      , writeBufferSize : options.writeBufferSize << 20
    })
  , timesStream = options.timingOutput
        && fs.createWriteStream(options.timingOutput, 'utf8')
//  , throughputStream = options.throughputOutput
//        && fs.createWriteStream(options.throughputOutput, 'utf8')

// make a 16 char padded key
function makeKey () {
  var r = Math.floor(Math.random() * options.num)
    , k = keyTmpl + r
  return k.substr(k.length - 16)
}

timesStream.write('Elapsed (ms), Entries, Bytes, Last 1000 Avg Time, MB/s\n')

setTimeout(function () { db.open(function (err) {
  if (err)
    throw err

  var inProgress  = 0
    , totalWrites = 0
    , totalBytes  = 0
    , startTime   = Date.now()
    , timesAccum  = 0
    , writeBuf    = ''
    , elapsed

  function report () {
    console.log(
        'Wrote'
      , options.num
      , 'entries in'
      , Math.floor((Date.now() - startTime) / 1000) + 's,'
      , (Math.floor((totalBytes / 1048576) * 100) / 100) + 'MB'
    )
    timesStream.end()

    du(options.db, function (err, size) {
      if (err)
        throw err
      console.log('Database size:', Math.floor(size / 1024 / 1024) + 'M')
    })
  }


  function write () {
    if (totalWrites++ == options.num) {
      db.close(function () {
        report(Date.now() - startTime)
      })
    }
    if (inProgress >= options.concurrency || totalWrites > options.num)
      return

    inProgress++

    if (totalWrites % 100000 === 0)
      console.log('' + inProgress, totalWrites, Math.round(totalWrites / options.num * 100) + '%')

    if (totalWrites % 1000 === 0) {
      elapsed = Date.now() - startTime
      timesStream.write(
                elapsed
        + ',' + totalWrites
        + ',' + totalBytes
        + ',' + Math.floor(timesAccum / 1000)
        + ',' + (Math.floor(((totalBytes / 1048576) / (elapsed / 1000)) * 100) / 100)
        + '\n')
      timesAccum = 0
      //timesStream.write(writeBuf)
      //writeBuf = ''
    }

    var time = process.hrtime()

    db.put(makeKey(), randomData.generate(options.valueSize), function (err) {
      if (err)
        throw err

      totalBytes += keyTmpl.length + options.valueSize
      timesAccum += process.hrtime(time)[1]
      //writeBuf += (Date.now() - startTime) + ',' + process.hrtime(time)[1] + '\n'
      inProgress--
      process.nextTick(write)
    })
  }

  for (var i = 0; i < options.concurrency; i++)
    write()
})}, 500)
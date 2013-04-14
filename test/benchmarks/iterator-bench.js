var leveldown = require('../../index.js')('/tmp/benchdb')
  , benchNextBuffering = function () {
      console.time('iterator.nextBuffering')
      var iterator = leveldown.iterator()
        , next = function () {
        iterator.nextBuffering(function(err, array) {
          if (err) throw err

          if (array.pop() === null)
            console.timeEnd('iterator.nextBuffering')
          else
            next()
        })
      }
      next()
    }
  , benchNext = function () {
    console.time('iterator.next')
    var iterator = leveldown.iterator()
      , next = function () {
          iterator.next(function(err, key, value) {
            if (err) throw err

            if (arguments.length === 0) {
              console.timeEnd('iterator.next')
              benchNextBuffering()
            } else
              next()
          })
        }
    next()
  }

leveldown.open(function () {
  console.time('setup')

  var i = 0
  , count = 0
  , L = 100000
  , done = function () {
      count++
      if (count === L) {
        console.timeEnd('setup')
        benchNext()
      }
    }

  for ( ; i < L; ++i) {
    leveldown.put(i.toString(), 'Hello, world!', done)
  }
})
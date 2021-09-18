'use strict'

// A special entry point to run certain tests with forced gc.

if (!global.gc) {
  console.error('Run with node --expose-gc')
  process.exit(1)
}

require('./cleanup-hanging-iterators-test')
require('./iterator-gc-test')
require('./chained-batch-gc-test')
require('./clear-gc-test')

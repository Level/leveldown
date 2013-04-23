const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')
    , abstract = require('abstract-leveldown/abstract/iterator-test')

if (require.main === module)
  abstract.all(leveldown, test, testCommon)
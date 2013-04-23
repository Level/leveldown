const test       = require('tap').test
    , testCommon = require('./common')
    , leveldown  = require('../')
    , abstract = require('abstract-leveldown/abstract/open-test')

if (require.main === module)
  abstract.all(leveldown, test, testCommon)
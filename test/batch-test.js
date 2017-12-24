const test       = require('tape')
    , leveldown  = require('..')
    , abstract   = require('abstract-leveldown/abstract/batch-test')

abstract.all(leveldown, test)

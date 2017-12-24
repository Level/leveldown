const test       = require('tape')
    , leveldown  = require('..')
    , abstract   = require('abstract-leveldown/abstract/open-test')

abstract.all(leveldown, test)

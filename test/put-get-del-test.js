const test       = require('tape')
    , leveldown  = require('..')
    , fs         = require('fs')
    , path       = require('path')
    , testBuffer = fs.readFileSync(path.join(__dirname, 'data/testdata.bin'))
    , abstract   = require('abstract-leveldown/abstract/put-get-del-test')

abstract.all(leveldown, test, testBuffer)

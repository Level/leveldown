const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/test/put-get-del-test')

abstract.all(leveldown, test)

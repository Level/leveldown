const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/test/put-test')

abstract.all(leveldown, test)

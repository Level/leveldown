const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/abstract/put-test')

abstract.all(leveldown, test)

const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/abstract/batch-test')

abstract.all(leveldown, test)

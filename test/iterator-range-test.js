const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/test/iterator-range-test')

abstract.all(leveldown, test)

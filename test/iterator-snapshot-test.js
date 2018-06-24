const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/abstract/iterator-snapshot-test')

abstract.all(leveldown, test)

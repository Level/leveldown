const test = require('tape'),
  leveldown = require('..'),
  abstract = require('abstract-leveldown/abstract/iterator-range-test')

abstract.all(leveldown, test)

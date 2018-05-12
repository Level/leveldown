const test = require('tape'),
  leveldown = require('..'),
  abstract = require('abstract-leveldown/abstract/chained-batch-test')

abstract.all(leveldown, test)

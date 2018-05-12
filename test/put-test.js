const test = require('tape'),
  leveldown = require('..'),
  abstract = require('abstract-leveldown/abstract/put-test')

abstract.all(leveldown, test)

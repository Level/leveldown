const test = require('tape'),
  leveldown = require('..'),
  abstract = require('abstract-leveldown/abstract/get-test')

abstract.all(leveldown, test)

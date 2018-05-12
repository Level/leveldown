const test = require('tape'),
  leveldown = require('..'),
  abstract = require('abstract-leveldown/abstract/put-get-del-test')

abstract.all(leveldown, test)

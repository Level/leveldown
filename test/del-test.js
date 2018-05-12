const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/abstract/del-test')

abstract.all(leveldown, test)

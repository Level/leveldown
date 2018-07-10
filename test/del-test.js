const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/test/del-test')

abstract.all(leveldown, test)

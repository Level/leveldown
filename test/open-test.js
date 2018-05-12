const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/abstract/open-test')

abstract.all(leveldown, test)

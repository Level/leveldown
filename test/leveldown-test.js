const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/abstract/leveldown-test')

abstract.args(leveldown, test)

const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/abstract/get-test')

abstract.all(leveldown, test)

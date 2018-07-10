const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/test/get-test')

abstract.all(leveldown, test)

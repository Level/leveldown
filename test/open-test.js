const test = require('tape')
const leveldown = require('..')
const abstract = require('abstract-leveldown/test/open-test')

abstract.all(leveldown, test)

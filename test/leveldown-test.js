const test = require('tape')
const testCommon = require('./common')
const abstract = require('abstract-leveldown/test/leveldown-test')

abstract.args(testCommon.factory, test)

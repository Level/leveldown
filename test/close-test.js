const test = require('tape')
const testCommon = require('./common')
const abstract = require('abstract-leveldown/test/close-test')

test('setUp', testCommon.setUp)

abstract.close(testCommon.factory, test, testCommon)

test('tearDown', testCommon.tearDown)

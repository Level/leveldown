0.1.4 @ Mar 11 2013
===================
  * return error when batch ops contain null or undefined (@rvagg / @ralphtheninja / @dominictarr) (additional tests in LevelUP for this)

0.1.3 @ Mar 9 2013
==================
  * add 'standalone_static_library':1 in dependency gyp files to fix SmartOS build problems (@wolfeidau)

0.1.2 @ Jan 25 2013
===================
  * upgrade to LevelDB 1.9.0, fairly minor changes since 1.7.0 (@rvagg)
  * upgrade to Snappy 1.1.0, changes block size to improve compression ~3%, slight decrease in speed (@rvagg)

0.1.1 @ Jan 25 2013
===================
  * compile error on Mac OS (@kesla / @rvagg)

0.1.0 @ Jan 24 2013
===================
  * change API to export single function `levelup()` (@rvagg)
  * move `createIterator()` to `levelup#iterator()` (@rvagg)
  * make all `options` arguments optional (@rvagg)
  * argument number & type checking on all methods (@rvagg)
  * stricter checking on key & value types, String/Object.toString()/Buffer, non-zero-length (@rvagg)
  * remove `use namespace` and add `namespace leveldown` everywhere (@rvagg)
  * race condition in Iterator end()/next() fix, merged from LevelUP (@ralphtheninja / @rvagg)
  * add complete, independent test suite (@rvagg)

0.0.1 & 0.0.2 @ Jan 2013
========================
  * finalise rename of internal components to LevelDOWN, removing LevelUP references (@rvagg)
  * complete documentation of current API (@rvagg)

0.0.0 @ Jan 06 2013
===================
  * extracted from LevelUP as stand-alone package (@rvagg)
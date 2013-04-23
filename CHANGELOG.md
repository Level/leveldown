0.2.2 @ (unreleased)
===================
  * Pull API tests up into AbstractLevelDOWN, require it to run the
tests. AbstractLevelDOWN can now be used to test LevelDOWN-compliant
APIs. (@maxogden)

0.2.1 @ Apr 8 2013
==================
  * Start on correct value when reverse=true, also handle end-of-store case #27 (@kesla)
  * Ignore empty string/buffer start/end options on iterators (@kesla)
  * Macro cleanup, replace some with static inline functions (@rvagg)

0.2.0 @ Mar 30 2013
===================
  * Windows support--using a combination of libuv and Windows-specific code. See README for details about what's required (@rvagg)
  * leveldown.destroy(location, callback) to delete an existing LevelDB store, exposes LevelDB.DestroyDB() (@rvagg)
  * leveldown.repair(location, callback) to repair an existing LevelDB store, exposes LevelDB.RepairDB() (@rvagg)
  * advanced options: writeBufferSize, blockSize, maxOpenFiles, blockRestartInterval, exposes LevelDB options (@rvagg)
  * chained-batch operations. Argument-less db.batch() will return a new Batch object that can .put() and .del() and then .write(). API in flux so not documented yet. (@juliangruber / @rvagg)
  * auto-cleanup iterators that are left open when you close a database; any iterators left open when you close a database instance will kill your process so we now keep track of iterators and auto-close them before a db.close completes.
  * Node 0.11 support (no compile warnings)

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

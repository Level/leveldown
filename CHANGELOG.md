0.7.0 @ Aug 11 2013
===================
  * Added @pgte to contributors list
  * (very) Minor perf improvements in C++ (@mscdex)
  * Use NAN <https://github.com/rvagg/nan> for Node 0.8->0.11 compatibility

0.6.2 @ Jul 07 2013
===================
  * Compatibility for Node 0.11.3, breaks compatibility with 0.11.2

0.6.1 @ Jun 15 2013
===================
  * Fix broken Windows compile, apply port-uv patch to LevelDB's port.h (@rvagg)

0.6.0 @ Jun 14 2013
===================
  * Upgrade to LevelDB 1.11.0, some important bugfixes: https://groups.google.com/forum/#!topic/leveldb/vS1JvmGlp4E

0.5.0 @ May 18 2013
===================
  * Bumped major version for db.getProperty() addition (should have been done in 0.4.4) (@rvagg)
  * Disallow batch() operations after a write() (@rvagg)

0.4.4 @ May 18 2013
===================
  * db.getProperty() implemented, see README for details (@rvagg)
  * More work on memory management, including late-creation of Persistent handles (@rvagg)

0.4.3 @ May 18 2013
===================
  * Better memory leak fix (@rvagg)

0.2.2 @ May 17 2013
===================
  * BACKPORT memory leak fixes (@rvagg)

0.4.2 @ May 17 2013
===================
  * Same memory leak fixes as 0.4.1, properly applied to batch() operations too (@rvagg)

0.4.1 @ May 17 2013
===================
  * Fix memory leak caused when passing String objects in as keys and values, conversion to Slice created new char[] but wasn't being disposed. Buffers are automatically disposed. (@rvagg, reported by @kylegetson levelup/#140)

0.4.0 @ May 15 2013
===================
  * Upgrade to LevelDB 1.10.0, fairly minor changes, mostly bugfixes see https://groups.google.com/forum/#!topic/leveldb/O2Zdbi9Lrao for more info (@rvagg)

0.3.1 @ May 14 2013
===================
  * Don't allow empty batch() operations through to LevelDB, on chained of array forms (@rvagg)

0.3.0 (& 0.2.2) @ May 14 2013
===================
  * Pull API tests up into AbstractLevelDOWN, require it to run the tests. AbstractLevelDOWN can now be used to test LevelDOWN-compliant APIs. (@maxogden)
  * Change Iterator methods to return errors on the callbacks rather than throw (@mcollina & @rvagg)

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

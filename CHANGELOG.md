Note that we have filtered out commits related to new tags, updating changelog and we're also not listing any merge commits, i.e. we are only listing things that changed between versions.

### 1.5.0 September 27 2016

* [[`a3fe375834`](https://github.com/level/leveldown/commit/a3fe375834)] - bump dependencies (Lars-Magnus Skog)
* [[`c02a3ad9e7`](https://github.com/level/leveldown/commit/c02a3ad9e7)] - chmod 644 leveldown dep (Julian Gruber)
* [[`4976d49a5f`](https://github.com/level/leveldown/commit/4976d49a5f)] - remove leveldb .travis.yml (Julian Gruber)
* [[`a0f91ad497`](https://github.com/level/leveldown/commit/a0f91ad497)] - upgrade nan, remove Function::NewInstance() deprecation warnings (Rod Vagg)
* [[`21dc91ebe6`](https://github.com/level/leveldown/commit/21dc91ebe6)] - apply port_uv.h patch (Julian Gruber)
* [[`a39c3ea3f3`](https://github.com/level/leveldown/commit/a39c3ea3f3)] - remove deleted file from leveldb.gyp (Julian Gruber)
* [[`a8b9f009c4`](https://github.com/level/leveldown/commit/a8b9f009c4)] - import leveldb-1.19 and point bindings file to it (Julian Gruber)

### 1.4.6 April 29 2016

* [[`267586b02b`](https://github.com/level/leveldown/commit/267586b02b)] - bump nan to 2.3.0 to remove deprecated calls (Lars-Magnus Skog)
* [[`1ef0452f81`](https://github.com/level/leveldown/commit/1ef0452f81)] - add node 6 to travis (Lars-Magnus Skog)

### 1.4.5 April 18 2016

* [[`df2b52217b`](https://github.com/level/leveldown/commit/df2b52217b)] - steal chjjs script for checking memory consumption (Lars-Magnus Skog)
* [[`b86e72bc76`](https://github.com/level/leveldown/commit/b86e72bc76)] - delete calls. (Christopher Jeffrey)
* [[`ce3eb77faf`](https://github.com/level/leveldown/commit/ce3eb77faf)] - optimize iterators by avoiding handles. (Christopher Jeffrey)
* [[`259a5b7e64`](https://github.com/level/leveldown/commit/259a5b7e64)] - free start slices on conflicting options. (Christopher Jeffrey)
* [[`ab2d68db3d`](https://github.com/level/leveldown/commit/ab2d68db3d)] - dispose of unused slices. (Christopher Jeffrey)
* [[`160e0b2fba`](https://github.com/level/leveldown/commit/160e0b2fba)] - release iterator snapshots. (Christopher Jeffrey)
* [[`bd95ec136f`](https://github.com/level/leveldown/commit/bd95ec136f)] - bump dependencies (Lars-Magnus Skog)
* [[`547681f33a`](https://github.com/level/leveldown/commit/547681f33a)] - fix iterator leak. (Christopher Jeffrey)
* [[`9cf6ea07d0`](https://github.com/level/leveldown/commit/9cf6ea07d0)] - add handlescopes to fix potential memory leaks. (Christopher Jeffrey)
* [[`a02ac81bc9`](https://github.com/level/leveldown/commit/a02ac81bc9)] - Fix repair-test for multilang windows (Vincent Weevers)
* [[`553e631e9b`](https://github.com/level/leveldown/commit/553e631e9b)] - gitignore Release folder for Windows (Vincent Weevers)
* [[`415b6fc3c6`](https://github.com/level/leveldown/commit/415b6fc3c6)] - remove unsupported versions (Lars-Magnus Skog)
* [[`e312f26add`](https://github.com/level/leveldown/commit/e312f26add)] - 2015 -> 2016 (Lars-Magnus Skog)
* [[`fa9fe64cd8`](https://github.com/level/leveldown/commit/fa9fe64cd8)] - chore(package): update prebuild to version 3.0.0 (greenkeeperio-bot)
* [[`2ae07c3b2c`](https://github.com/level/leveldown/commit/2ae07c3b2c)] - repair error is different on windows (Lars-Magnus Skog)

### 1.4.4 January 1 2016

* [[`63d9dfc4f9`](https://github.com/level/leveldown/commit/63d9dfc4f9)] - Fixes #247 (Oguz Bastemur)
* [[`6566032b2e`](https://github.com/level/leveldown/commit/6566032b2e)] - chore(package): update nan to version 2.2.0 (greenkeeperio-bot)
* [[`7535949bbf`](https://github.com/level/leveldown/commit/7535949bbf)] - use prebuild --install (Lars-Magnus Skog)
* [[`7e5c2fa836`](https://github.com/level/leveldown/commit/7e5c2fa836)] - chore(package): update prebuild to version 2.8.0 (greenkeeperio-bot)
* [[`041be035c7`](https://github.com/level/leveldown/commit/041be035c7)] - README fixes (Lars-Magnus Skog)
* [[`9af352465b`](https://github.com/level/leveldown/commit/9af352465b)] - chore(package): update rimraf to version 2.5.0 (greenkeeperio-bot)
* [[`c858adc32a`](https://github.com/level/leveldown/commit/c858adc32a)] - chore(package): update tape to version 4.3.0 (greenkeeperio-bot)

### 1.4.3 December 14 2015

* [[`3303be2f5d`](https://github.com/level/leveldown/commit/3303be2f5d)] - bump dependencies (Lars-Magnus Skog)
* [[`fc46cdc81f`](https://github.com/level/leveldown/commit/fc46cdc81f)] - add dependency badge (Lars-Magnus Skog)
* [[`f9d80f68c4`](https://github.com/level/leveldown/commit/f9d80f68c4)] - bump prebuild for --all functionality (Lars-Magnus Skog)
* [[`0fa159729f`](https://github.com/level/leveldown/commit/0fa159729f)] - Fixes https://github.com/Level/leveldown/issues/196 (Oguz Bastemur)
* [[`173454a31e`](https://github.com/level/leveldown/commit/173454a31e)] - test on node 5 (Lars-Magnus Skog)
* [[`369352d0a8`](https://github.com/level/leveldown/commit/369352d0a8)] - prebuild abi 47 (node 5.0) (Lars-Magnus Skog)

### 1.4.2 October 21 2015

* [[`8f81825384`](https://github.com/level/leveldown/commit/8f81825384)] - test on latest node versions (Lars-Magnus Skog)
* [[`7d85c74057`](https://github.com/level/leveldown/commit/7d85c74057)] - require at start, not later (Tim Kuijsten)
* [[`949a2c26d6`](https://github.com/level/leveldown/commit/949a2c26d6)] - add node v4.0.0 to travis (Lars-Magnus Skog)
* [[`8f3863b6dd`](https://github.com/level/leveldown/commit/8f3863b6dd)] - add node v4.0.0 (abi 46) (Lars-Magnus Skog)
* [[`8b8c29e147`](https://github.com/level/leveldown/commit/8b8c29e147)] - Document prebuild and explain different compile steps (Lars-Magnus Skog)
* [[`b136753695`](https://github.com/level/leveldown/commit/b136753695)] - update prebuild for --debug support (Lars-Magnus Skog)
* [[`0f6a776639`](https://github.com/level/leveldown/commit/0f6a776639)] - update prebuild for --strip functionality (Lars-Magnus Skog)
* [[`372c09c768`](https://github.com/level/leveldown/commit/372c09c768)] - add @kkoopa's travis fix (Mathias Buus)

### 1.4.1 August 15 2015

* [[`3d9d73ff64`](https://github.com/level/leveldown/commit/3d9d73ff64)] - add abi 45 to prebuild (Mathias Buus)
* [[`2bc173b08d`](https://github.com/level/leveldown/commit/2bc173b08d)] - add iojs 3.0 to travis (Mathias Buus)
* [[`ff8a40b378`](https://github.com/level/leveldown/commit/ff8a40b378)] - Merge pull request #195 from Level/iojs-v3-nan-next (Mathias Buus)
* [[`89df0bd046`](https://github.com/level/leveldown/commit/89df0bd046)] - replace missing Handle<...> with Local<...> (Mathias Buus)
* [[`b7272db9f1`](https://github.com/level/leveldown/commit/b7272db9f1)] - upgrade to NAN v2 (Rod Vagg)
* [[`15e01d4c6d`](https://github.com/level/leveldown/commit/15e01d4c6d)] - update prebuild (LM)

### 1.4.0 July 28 2015

* [[`8b6c3cf37e`](https://github.com/level/leveldown/commit/8b6c3cf37e)] - add .prebuildrc, update travis and prebuild, add prebuild npm script (LM)
* [[`c580c36271`](https://github.com/level/leveldown/commit/c580c36271)] - clean up (LM)
* [[`de7ff9c426`](https://github.com/level/leveldown/commit/de7ff9c426)] - add rebuild npm script (LM)
* [[`0899a359da`](https://github.com/level/leveldown/commit/0899a359da)] - no longer need node-gyp directly (LM)
* [[`a3f429da46`](https://github.com/level/leveldown/commit/a3f429da46)] - switch from node-pre-gyp to prebuild (LM)
* [[`269c77e5bc`](https://github.com/level/leveldown/commit/269c77e5bc)] - node-pre-gyp should build binaries on travis (LM)
* [[`da1d679540`](https://github.com/level/leveldown/commit/da1d679540)] - Remove hash_test.cc source (Michael Nisi)

### 1.3.0 June 16 2015

* [[`9ed4d812bc`](https://github.com/level/leveldown/commit/9ed4d812bc)] - Added LEVELDB_PLATFORM_UV to LevelDB port.h to fix test. (Braydon Fuller)
* [[`f9fa2c06b0`](https://github.com/level/leveldown/commit/f9fa2c06b0)] - Upgrade leveldb to 1.18.0 (Braydon Fuller)
* [[`8a57fab2f8`](https://github.com/level/leveldown/commit/8a57fab2f8)] - build for for all different ABI versions (Lars-Magnus Skog)

### 1.2.2 June 2 2015

* [[`405a303c3b`](https://github.com/level/leveldown/commit/405a303c3b)] - ignore build-pre-gyp/ folder when publishing to npm (Lars-Magnus Skog)

### 1.2.1 June 2 2015

* [[`ddcc9ce303`](https://github.com/level/leveldown/commit/ddcc9ce303)] - Merge pull request #188 from ralphtheninja/prebuilt (Lars-Magnus Skog)
* [[`25d5f6206a`](https://github.com/level/leveldown/commit/25d5f6206a)] - use remote_path with node-pre-gyp to dodge preparing package.json every time (Lars-Magnus Skog)
* [[`78068f26ca`](https://github.com/level/leveldown/commit/78068f26ca)] - add more iojs versions (Lars-Magnus Skog)
* [[`ec81d77746`](https://github.com/level/leveldown/commit/ec81d77746)] - use node-gyp-install to make sure correct node-gyp headers are downloaded (Lars-Magnus Skog)
* [[`245b54a4e6`](https://github.com/level/leveldown/commit/245b54a4e6)] - prepare v1.2.1 (Lars-Magnus Skog)

### 1.2.0 June 1 2015

* [[`a134de131d`](https://github.com/level/leveldown/commit/a134de131d)] - minor changes based on pr review (Mathias Buus)
* [[`8ded32b8d3`](https://github.com/level/leveldown/commit/8ded32b8d3)] - added iterator.seek() (Mathias Buus)
* [[`aa982df780`](https://github.com/level/leveldown/commit/aa982df780)] - change NOTICE to Prebuilt binaries section (Lars-Magnus Skog)
* [[`79412de76f`](https://github.com/level/leveldown/commit/79412de76f)] - solved conflict and updated leveldown-prebuilt to leveldown (Lars-Magnus Skog)

### 1.1.0 May 28 2015

* [[`d5da7fa357`](https://github.com/level/leveldown/commit/d5da7fa357)] - upgrade abstract-leveldown, mainly for .status (Julian Gruber)

### 1.0.7 May 27 2015

 * [[`61398a0056`](https://github.com/level/leveldown/commit/61398a0056)] - link to level/community (Lars-Magnus Skog)
 * [[`382a1a7fa3`](https://github.com/level/leveldown/commit/382a1a7fa3)] - add compression test suite (Julian Gruber)
 * [[`139db7bc7f`](https://github.com/level/leveldown/commit/139db7bc7f)] - use const reference instead of by value in Database constructor (Lars-Magnus Skog)
 * [[`b56a86323e`](https://github.com/level/leveldown/commit/b56a86323e)] - refactor NanNew() on strings into option value functions (Lars-Magnus Skog)
 * [[`ca1f4746c4`](https://github.com/level/leveldown/commit/ca1f4746c4)] - refactor BooleanOptionValue (Lars-Magnus Skog)
 * [[`56def2d7c8`](https://github.com/level/leveldown/commit/56def2d7c8)] - NanUInt32OptionValue -> UInt32OptionValue (Lars-Magnus Skog)
 * [[`39c614a24f`](https://github.com/level/leveldown/commit/39c614a24f)] - NanBooleanOptionValue -> BooleanOptionValue (Lars-Magnus Skog)
 * [[`fcdc46183e`](https://github.com/level/leveldown/commit/fcdc46183e)] - simplify location logic, let Database take care of allocation (Lars-Magnus Skog)
 * [[`8cb90e6b6d`](https://github.com/level/leveldown/commit/8cb90e6b6d)] - update abstract-leveldown (Lars-Magnus Skog)
 * [[`f70b6576e7`](https://github.com/level/leveldown/commit/f70b6576e7)] - update .travis.yml (nvm works on travis now) (Lars-Magnus Skog)
 * [[`007550e7f7`](https://github.com/level/leveldown/commit/007550e7f7)] - 1.0.6 (Lars-Magnus Skog)

### 1.0.5/1.0.6 May 5 2015

* [[`9064099fe7`](https://github.com/level/leveldown/commit/9064099fe7)] - pass db to abstractIterator so gc keeps it (Julian Gruber)

### 1.0.4 May 5 2015

 * [[`b550c98291`](https://github.com/level/leveldown/commit/b550c98291)] - update nan for iojs 2.0.0 (Lars-Magnus Skog)

### 1.0.3 May 2 2015

 * [[`82479b689f`](https://github.com/level/leveldown/commit/82479b689f)] - tap -> tape + faucet (Lars-Magnus Skog)
 * [[`ca9101542a`](https://github.com/level/leveldown/commit/ca9101542a)] - fix write-random.js, use leveldown instead of lmdb (Lars-Magnus Skog)
 * [[`03fbbfb99f`](https://github.com/level/leveldown/commit/03fbbfb99f)] - fix bench/db-bench.js (Lars-Magnus Skog)

### 1.0.2 Apr 26 2015
  * [[`8470a63678`](https://github.com/level/leveldown/commit/8470a63678)] - s/rvagg\/node-/level\// (Lars-Magnus Skog)
  * [[`9cbf592bea`](https://github.com/level/leveldown/commit/9cbf592bea)] - add documentation about snapshots (Max Ogden)
  * [[`b57827cd29`](https://github.com/level/leveldown/commit/b57827cd29)] - use n instead of nvm for working iojs support (Lars-Magnus Skog)
  * [[`a19927667a`](https://github.com/level/leveldown/commit/a19927667a)] - abstract-leveldown ~2.1.0 (ralphtheninja)
  * [[`95ccdf0850`](https://github.com/level/leveldown/commit/95ccdf0850)] - update logo and copyright (Lars-Magnus Skog)
  * [[`09e89d7abb`](https://github.com/level/leveldown/commit/09e89d7abb)] - updated my email (ralphtheninja)

### 1.0.1 Jan 16 2015
  * [[`6df3ecd6f5`](https://github.com/level/leveldown/commit/6df3ecd6f5)] - nan 1.5 for io.js support (Rod Vagg)
  * [[`5198231a88`](https://github.com/level/leveldown/commit/5198231a88)] - Fix LevelDB builds for modern gcc versions (Sharvil Nanavati)

### 1.0.0 Aug 26 2014
  * NAN@1.3 for Node 0.11.13+ support (@rvagg)
  * Allow writing empty values: null, undefined, '', [] and Buffer(0). Entries come out as '' or Buffer(0) (@ggreer, @juliangruber, @rvagg)
  * Fix clang build (@thlorenz)
  * Massive speed up of iterators by chunking reads (@kesla)
  * Wrap in abstract-leveldown for consistent type-checking across *DOWNs (@kesla)
  * Upgrade to LevelDB 1.17.0 (@kesla)
  * Minor memory leaks
  * Remove compile option that borked EL5 compiles
  * Switch to plain MIT license

### 0.10.2 @ Nov 30 2013

  * Apply fix by @rescrv for long-standing OSX corruption bug, https://groups.google.com/forum/#!topic/leveldb/GXhx8YvFiig (@rvagg / @rescrv)

### 0.10.1 @ Nov 21 2013

  * NAN@0.6 for Node@0.11.6 support, v8::Local<T>::New(val) rewritten to
    NanNewLocal<T>(val) (@rvagg)

### 0.10.0 @ Nov 18 2013

  * Fix array-batch memory leak, levelup/#171 (@rvagg)
  * Fix chained-batch write() segfaults, details in #73, (@rvagg and
    @mcollina)
  * Remove `Persistent` references for all `batch()` operations as
    `WriteBatch` takes an explicit copy of the data (@mcollina and
@rvagg)
  * Upgrade to Snappy 1.1.1 (@rvagg and @no9)
  * Upgrade to NAN@0.5.x (@rvagg)
  * Switch all `callback->Call()`s to `node::MakeCallback()` to properly
    support Node.js domains (@rvagg)
  * Properly enable compression by default (@Kyotoweb)
  * Enable LevelDB's BloomFilter (@Kyotoweb)
  * Upgrade to AbstractLevelDOWN@0.11.x for testing (@rvagg)
  * Add new simple batch() leak tester (@rvagg)

### 0.9.2 @ Nov 02 2013

  * Minor fixes to support Node 0.11.8 and new Linux gcc (warnings) (@rvagg)

### 0.9.1 @ Oct 03 2013

  * Include port_uv.h for Windows compile, added test to suite to make sure this happens every time LevelDB is upgraded (@rvagg)

### 0.9.0 @ Oct 01 2013

  * Upgrade from LevelDB@0.11.0 to LevelDB@0.14.0, includes change from .sst to .ldb file extension for SST files (@rvagg)

### 0.8.3 @ Sept 18 2013

  * Upgrade to nan@0.4.0, better support for latest Node master & support for installing within directory structures containing spaces in directory names (@rvagg)

### 0.8.2 @ Sept 2 2013

  * FreeBSD support (@rvagg, @kelexel)

### 0.8.1 @ Sept 1 2013

  * Fixed some minor V8-level leaks (@rvagg)

### 0.8.0 @ Aug 19 2013

  * Added `gt`, `lt`, `gte`, `lte` for iterators (@dominictarr)
  * Switch to NAN as an npm dependency (@rvagg)

### 0.7.0 @ Aug 11 2013

  * Added @pgte to contributors list
  * (very) Minor perf improvements in C++ (@mscdex)
  * Use NAN <https://github.com/rvagg/nan> for Node 0.8->0.11 compatibility

### 0.6.2 @ Jul 07 2013

  * Compatibility for Node 0.11.3, breaks compatibility with 0.11.2

### 0.6.1 @ Jun 15 2013

  * Fix broken Windows compile, apply port-uv patch to LevelDB's port.h (@rvagg)

### 0.6.0 @ Jun 14 2013

  * Upgrade to LevelDB 1.11.0, some important bugfixes: https://groups.google.com/forum/#!topic/leveldb/vS1JvmGlp4E

### 0.5.0 @ May 18 2013

  * Bumped major version for db.getProperty() addition (should have been done in 0.4.4) (@rvagg)
  * Disallow batch() operations after a write() (@rvagg)

### 0.4.4 @ May 18 2013

  * db.getProperty() implemented, see README for details (@rvagg)
  * More work on memory management, including late-creation of Persistent handles (@rvagg)

### 0.4.3 @ May 18 2013

  * Better memory leak fix (@rvagg)

### 0.2.2 @ May 17 2013

  * BACKPORT memory leak fixes (@rvagg)

### 0.4.2 @ May 17 2013

  * Same memory leak fixes as 0.4.1, properly applied to batch() operations too (@rvagg)

### 0.4.1 @ May 17 2013

  * Fix memory leak caused when passing String objects in as keys and values, conversion to Slice created new char[] but wasn't being disposed. Buffers are automatically disposed. (@rvagg, reported by @kylegetson levelup/#140)

### 0.4.0 @ May 15 2013

  * Upgrade to LevelDB 1.10.0, fairly minor changes, mostly bugfixes see https://groups.google.com/forum/#!topic/leveldb/O2Zdbi9Lrao for more info (@rvagg)

### 0.3.1 @ May 14 2013

  * Don't allow empty batch() operations through to LevelDB, on chained of array forms (@rvagg)

### 0.3.0 (& 0.2.2) @ May 14 2013

  * Pull API tests up into AbstractLevelDOWN, require it to run the tests. AbstractLevelDOWN can now be used to test LevelDOWN-compliant APIs. (@maxogden)
  * Change Iterator methods to return errors on the callbacks rather than throw (@mcollina & @rvagg)

0.2.1 @ Apr 8 2013
==================
  * Start on correct value when reverse=true, also handle end-of-store case #27 (@kesla)
  * Ignore empty string/buffer start/end options on iterators (@kesla)
  * Macro cleanup, replace some with static inline functions (@rvagg)

### 0.2.0 @ Mar 30 2013

  * Windows support--using a combination of libuv and Windows-specific code. See README for details about what's required (@rvagg)
  * leveldown.destroy(location, callback) to delete an existing LevelDB store, exposes LevelDB.DestroyDB() (@rvagg)
  * leveldown.repair(location, callback) to repair an existing LevelDB store, exposes LevelDB.RepairDB() (@rvagg)
  * advanced options: writeBufferSize, blockSize, maxOpenFiles, blockRestartInterval, exposes LevelDB options (@rvagg)
  * chained-batch operations. Argument-less db.batch() will return a new Batch object that can .put() and .del() and then .write(). API in flux so not documented yet. (@juliangruber / @rvagg)
  * auto-cleanup iterators that are left open when you close a database; any iterators left open when you close a database instance will kill your process so we now keep track of iterators and auto-close them before a db.close completes.
  * Node 0.11 support (no compile warnings)

### 0.1.4 @ Mar 11 2013

  * return error when batch ops contain null or undefined (@rvagg / @ralphtheninja / @dominictarr) (additional tests in LevelUP for this)

0.1.3 @ Mar 9 2013
==================
  * add 'standalone_static_library':1 in dependency gyp files to fix SmartOS build problems (@wolfeidau)

### 0.1.2 @ Jan 25 2013

  * upgrade to LevelDB 1.9.0, fairly minor changes since 1.7.0 (@rvagg)
  * upgrade to Snappy 1.1.0, changes block size to improve compression ~3%, slight decrease in speed (@rvagg)

### 0.1.1 @ Jan 25 2013

  * compile error on Mac OS (@kesla / @rvagg)

### 0.1.0 @ Jan 24 2013

  * change API to export single function `levelup()` (@rvagg)
  * move `createIterator()` to `levelup#iterator()` (@rvagg)
  * make all `options` arguments optional (@rvagg)
  * argument number & type checking on all methods (@rvagg)
  * stricter checking on key & value types, String/Object.toString()/Buffer, non-zero-length (@rvagg)
  * remove `use namespace` and add `namespace leveldown` everywhere (@rvagg)
  * race condition in Iterator end()/next() fix, merged from LevelUP (@ralphtheninja / @rvagg)
  * add complete, independent test suite (@rvagg)

### 0.0.1 & 0.0.2 @ Jan 2013

  * finalise rename of internal components to LevelDOWN, removing LevelUP references (@rvagg)
  * complete documentation of current API (@rvagg)

### 0.0.0 @ Jan 06 2013

  * extracted from LevelUP as stand-alone package (@rvagg)

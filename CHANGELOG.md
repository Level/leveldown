# Changelog

## [Unreleased]

## [3.0.0] - 2018-01-30

### Changed
* upgrade to `abstract-leveldown@4` (@ralphtheninja)
* upgrade to `prebuild@7` (@ralphtheninja)
* rewrite changelog (@ralphtheninja)

### Removed
* node 5 and 7 from travis (@ralphtheninja)

### Fixed
* make sure `destroy()` deletes `LevelDB`-only dir (@joyeecheung)

## [2.1.1] - 2017-12-02

### Fixed
* omit docs from `LevelDB` and test files from `Snappy` (@peakji)

## [2.1.0] - 2017-11-24

### Fixed
* fix `Android` compilation (@staltz)

## [2.0.2] - 2017-11-23

### Added
* `node@9` to travis and appveyor (@ralphtheninja)

### Changed
* upgrade to `nan@2.8` (@ralphtheninja)

## [2.0.1] - 2017-11-11

### Changed
* major `README` overhaul (@ralphtheninja, @vweevers)
* upgrade to `abstract-leveldown@3.0.0` (@vweevers)

### Fixed
* tweak typings (@sandersn)

## [2.0.0] - 2017-10-02

### Changed
* update `TypeScript` typings (@meirionhughes)

### Removed
* support for `node@0.12` from travis and appveyor (@ralphtheninja)

## [1.9.0] - 2017-09-28

### Added
* default export of `LevelDOWN` (@zixia)

## [1.8.0] - 2017-09-14

### Added
* node version badge (@vweevers)
* add Greenkeeper badge (@ralphtheninja)
* `-mfloat-abi=hard` compiler flag for `arm` (@maihde)
* initial `TypeScript` typings (@meirionhughes)
* @meirionhughes to contributors (@meirionhughes)

### Changed
* bump dependencies (@ralphtheninja)
* simplify appveyor (@vweevers)
* install latest `npm` on appveyor for `node@5` (@vweevers)

### Removed

### Fixed

## [1.7.2] - 2017-06-08

### Changed
* `iterator.next()` calls back with error if called after `iterator.end()` (@peakji)

### Fixed
* closing db with open iterator should not crash (@peakji)

## [1.7.1] - 2017-06-01

### Added
* `node@8` to travis and appveyor (@ralphtheninja)

### Changed
* skip installing latest `npm` on appveyor (@ralphtheninja)

## [1.7.0] - 2017-05-17

### Changed
* bumped version (@ralphtheninja)

## [1.7.0-0] - 2017-04-12

### Added
* add support for `OpenBSD` (@qbit)

### Changed
* upgrade to `LevelDB@1.20` (@peakji)
* upgrade to `Snappy@1.1.4` (@peakji)
* bump dependencies (@ralphtheninja)

## [1.6.0] - 2017-02-02

### Added
* `db.compactRange(start, end, cb)` (@bookchin)
* @bookchin to contributors (@bookchin)

### Changed
* update `prebuild`, `prebuild-install` and `prebuild-ci` (@juliangruber)
* update `nan` (@ralphtheninja)

## [1.5.3] - 2016-12-30

### Added
* restore `node@5` to appveyor (@vweevers)

## [1.5.2] - 2016-12-29

### Added
* restore `node@5` to travis (@ralphtheninja)

## [1.5.1] - 2016-12-27

### Added
* add `node@7` to travis (@ralphtheninja)
* add `AppVeyor` so we can build on Windows (@vweevers)
* add OSX to travis (@juliangruber)
* add `prebuild-ci` so we can automatically make prebuilt binaries (@juliangruber)

### Changed
* enable `iterator.seek()` with buffers (@peakji)

### Removed
* remove `node@0.10` from travis (@ralphtheninja)
* remove `node@5` from travis (@ralphtheninja)

## [1.5.0] - 2016-09-27

### Changed
* upgrade to `LevelDB@1.19` (@juliangruber)
* bump dependencies (@ralphtheninja)
* upgrade to `nan@2.4.0`, remove `Function::NewInstance()` deprecation warnings (@rvagg)

### Removed
* remove deleted file from `leveldb.gyp` (@juliangruber)

## [1.4.6] - 2016-04-29

### Added
* add node 6 to travis (@ralphtheninja)

### Changed
* upgrade to `nan@2.3.0` to remove deprecated calls (@ralphtheninja)

## [1.4.5] - 2016-04-18

### Added
* steal chjjs script for checking memory consumption (@ralphtheninja)

### Changed
* bump dependencies (@ralphtheninja)
* gitignore Release folder for Windows (@vweevers)
* update copyright year 2015 -> 2016 (@ralphtheninja)
* optimize iterators by avoiding handles (@chjj)

### Removed
* remove unsupported versions from travis (@ralphtheninja)

### Fixed
* fix delete calls not using `[]` (@chjj)
* free start slices on conflicting options (@chjj)
* dispose of unused slices (@chjj)
* release iterator snapshots (@chjj)
* fix iterator leak (@chjj)
* add handlescopes to fix potential memory leaks (@chjj)
* fix repair-test for multilang windows (@vweevers)
* repair error is different on windows (@ralphtheninja)

## [1.4.4] - 2016-01-25

### Changed
* update dependencies (@ralphtheninja)
* use `prebuild --install` (@ralphtheninja)
* `README` fixes (@ralphtheninja)

### Fixed
* fix build problems on windows https://github.com/Level/leveldown/issues/247 (@obastemur)

## [1.4.3] - 2015-12-14

### Added
* `node@5` to travis (@ralphtheninja)

### Changed
* bump dependencies (@ralphtheninja)
* add dependency badge (@ralphtheninja)
* bump `prebuild` for `--all` functionality (@ralphtheninja)
* prebuild abi 47 (node 5.0) (@ralphtheninja)

### Fixed
* fix build problems on windows https://github.com/Level/leveldown/issues/196 (@obastemur)

## [1.4.2] - 2015-10-21

### Added
* add `node@4` to prebuild config (abi 46) (@ralphtheninja)
* add `node@4` to travis (@ralphtheninja)

### Changed
* test on latest node versions (@ralphtheninja)
* document `prebuild` and explain different compile steps (@ralphtheninja)
* update prebuild for --debug and --strip support (@ralphtheninja)
* require `fast-future` at start, not later (@timkuijsten)
* add @kkoopa's travis fix (@mafintosh)

## [1.4.1] - 2015-08-15

### Added
* add abi 45 to `prebuild` (@mafintosh)
* add `io.js@3.0` to travis (@mafintosh)

### Changed
* update `prebuild` (@ralphtheninja)
* replace missing `Handle<>` with `Local<>` (@mafintosh)
* upgrade to `nan@2` (@rvagg)

## [1.4.0] - 2015-07-28

### Added
* add `.prebuildrc` update travis and prebuild, add prebuild npm script (@ralphtheninja)
* add rebuild npm script (@ralphtheninja)

### Changed
* switch from `node-pre-gyp` to `prebuild` (@ralphtheninja)

### Removed
* no longer need `node-gyp` directly (@ralphtheninja)

### Fixed

## [1.3.1-0] - 2015-07-20

### Changed
* `node-pre-gyp` should build binaries on travis (@ralphtheninja)

### Removed
* remove `hash_test.cc` source (@michaelnisi)

## [1.3.0] - 2015-06-16

### Added
* `io.js@1.0` to travis (@ralphtheninja)

### Changed
* upgrade to `LevelDB@1.18.0` (Braydon Fuller)

### Removed
* `io.js@2.0` from travis (@ralphtheninja)
* `io.js@2.1` from travis (@ralphtheninja)

### Fixed
* added `LEVELDB_PLATFORM_UV` to `LevelDB` `port.h` to fix test (Braydon Fuller)

## [1.2.2] - 2015-06-02

### Fixed
* ignore `build-pre-gyp/` folder when publishing to npm (@ralphtheninja)

## [1.2.1] - 2015-06-01

### Changed
* use `remote_path` with `node-pre-gyp` to dodge preparing `package.json` every time (@ralphtheninja)
* add more `io.js` versions (@ralphtheninja)
* use `node-gyp-install` to make sure correct `node-gyp` headers are downloaded (@ralphtheninja)

## [1.2.0] - 2015-06-01

### Added
* added `iterator.seek()` (@mafintosh)

## [1.1.0] - 2015-05-28

### Changed
* upgrade `abstract-leveldown`, mainly for `.status` (@juliangruber)

## [1.0.7] - 2015-05-27

### Added
* add compression test suite (@juliangruber)
* link to `level/community` (@ralphtheninja)

### Changed
* use const reference instead of by value in `Database` constructor (@ralphtheninja)
* refactor `NanNew()` on strings into option value functions (@ralphtheninja)
* refactor `BooleanOptionValue` (@ralphtheninja)
* `NanUInt32OptionValue` -> `UInt32OptionValue` (@ralphtheninja)
* `NanBooleanOptionValue` -> `BooleanOptionValue` (@ralphtheninja)
* simplify location logic, let `Database` take care of allocation (@ralphtheninja)
* update `abstract-leveldown` (@ralphtheninja)
* update `.travis.yml`, nvm works on travis now (@ralphtheninja)

## [1.0.6] - 2015-05-05

### Changed
* bumped version (@ralphtheninja)

## [1.0.5] - 2015-05-05

### Fixed
* pass db to `AbstractIterator` so gc keeps it (@juliangruber)

## [1.0.4] - 2015-05-05

### Changed
* update `nan` for iojs 2.0.0 (@ralphtheninja)

## [1.0.3] - 2015-05-02

### Changed
* `tap` -> `tape` + `faucet` (@ralphtheninja)

### Fixed
* fix `write-random.js`, use `leveldown` instead of `lmdb` (@ralphtheninja)
* fix `bench/db-bench.js` (@ralphtheninja)

## [1.0.2] - 2015-04-26

### Added
* add documentation about snapshots (@maxogden)

### Changed
* update logo and copyright (@ralphtheninja)
* s/rvagg\/node-/level\// (@ralphtheninja)
* use `n` instead of `nvm` for working `io.js` support (@ralphtheninja)
* update to `abstract-leveldown@2.1.0` (@ralphtheninja)

## [1.0.1] - 2015-01-16

### Changed
* upgrade to `nan@1.5` for `io.js` support (@rvagg)

### Fixed
* fix `LevelDB` builds for modern gcc versions (@sharvil)

## [1.0.0] - 2014-08-26

### Changed
* upgrade to `nan@1.3` for Node 0.11.13+ support (@rvagg)
* upgrade to `LevelDB@1.17.0` (@kesla)
* allow writing empty values: null, undefined, '', [] and Buffer(0). Entries come out as '' or Buffer(0) (@ggreer, @juliangruber, @rvagg)
* massive speed up of iterators by chunking reads (@kesla)
* wrap in abstract-leveldown for consistent type-checking across \*DOWNs (@kesla)
* switch to plain MIT license (@andrewrk)

### Removed
* remove compile option that borked EL5 compiles (@rvagg)

### Fixed
* fix clang build (@thlorenz)
* fix minor memory leaks in options attributes (@ggreer)

## [0.10.6] - 2016-01-07

### Added
* add `iojs`, `node@4` and `node@5` to travis (@ralphtheninja)

### Changed
* upgrade to `nan@2.1.x` for `node@4` and `node@5` support (@ralphtheninja)

### Removed
* remove `node@0.8` from travis (@ralphtheninja)

## [0.10.5] - 2015-05-05

### Changed
* upgrade to `nan@1.8.x` for `iojs` support (@ralphtheninja)

## [0.10.4] - 2015-02-07

### Changed
* bumped version (@rvagg)

## [0.10.3] - 2015-02-07

### Changed
* upgrade to `nan@1.5` (@mcollina)

## [0.10.2] - 2013-11-30

### Fixed
* apply fix by @rescrv for long-standing [OSX corruption bug](https://groups.google.com/forum/#!topic/leveldb/GXhx8YvFiig) (@rvagg / @rescrv)

## [0.10.1] - 2013-11-21

### Changed
* upgrade to `nan@0.6` for Node@0.11.6 support, `v8::Local<T>::New(val)` rewritten to `NanNewLocal<T>(val)` (@rvagg)

**Historical Note** From this release and onward, tags in git start with the prefix `v`, i.e. this release corresponds to the tag `v0.10.1`.

## [0.10.0] - 2013-11-18

### Added
* add new simple batch() leak tester (@rvagg)

### Changed
* upgrade to `Snappy@1.1.1` (@rvagg and @No9)
* upgrade to `nan@0.5.x` (@rvagg)
* upgrade to `abstract-leveldown@0.11.x` for testing (@rvagg)
* switch all `callback->Call()`s to `node::MakeCallback()` to properly support Node.js domains (@rvagg)
* enable LevelDB's BloomFilter (@Kyotoweb)
* properly enable compression by default (@Kyotoweb)

### Removed
* remove `Persistent` references for all `batch()` operations as `WriteBatch` takes an explicit copy of the data (@mcollina and @rvagg)

### Fixed
* fix array-batch memory leak, levelup/#171 (@rvagg)
* fix chained-batch `write()` segfaults, details in #73, (@rvagg and @mcollina)

## [0.9.2] - 2013-11-02

### Fixed
* minor fixes to support `Node@0.11.8` and new Linux gcc (warnings) (@rvagg)

## [0.9.1] - 2013-10-03

### Fixed
* include `port_uv.h` for Windows compile, added test to suite to make sure this happens every time `LevelDB` is upgraded (@rvagg)

## [0.9.0] - 2013-10-01

### Changed
* upgrade to `LevelDB@0.14.0`, includes change from .sst to .ldb file extension for SST files (@rvagg)

## [0.8.3] - 2013-09-18

### Changed
* upgrade to `nan@0.4.0`, better support for latest Node master & support for installing within directory structures containing spaces in directory names (@rvagg)

**Historical Note** The diff between this version and the previous shows `0.8.4` in the commit message. This is incorrect, since that version was never released.

## [0.8.2] - 2013-09-01

### Added
* support for `FreeBSD` (@rvagg, @kelexel)

## [0.8.1] - 2013-09-01

### Added
* @substack to contributors (@rvagg)

### Fixed
* minor V8-level leaks (@rvagg)

## [0.8.0] - 2013-08-26

### Added
* added `gt`, `lt`, `gte`, `lte` for iterators (@dominictarr)

### Changed
* switch to `nan` as an npm dependency (@rvagg)

## [0.7.0] - 2013-08-11

### Added
* @pgte to contributors (@rvagg)

### Changed
* use [nan](https://github.com/rvagg/nan) for Node 0.8->0.11 compatibility (@rvagg)
* minor perf improvements in C++ (@mscdex)

**Historical Note** We started using `nan` in the form of a local `src/nan.h` header file.

## [0.6.2] - 2013-07-07

### Changed
* compatibility for `Node@0.11.3`, breaks compatibility with 0.11.2

## [0.6.1] - 2013-06-15

### Fixed
* fix broken Windows compile, apply port-uv patch to `LevelDB`'s port.h (@rvagg)

## [0.6.0] - 2013-06-14

### Changed
* upgrade to `LevelDB@1.11.0`, some [important bugfixes](https://groups.google.com/forum/#!topic/leveldb/vS1JvmGlp4E) (@rvagg)

## [0.5.0] - 2013-05-21

### Changed
* bumped major version for `db.getProperty()` addition (should have been done in 0.4.4) (@rvagg)
* disallow `batch()` operations after a `write()` (@rvagg)

## [0.4.4] - 2013-05-18

### Added
* `db.getProperty()` implemented, see README for details (@rvagg)

### Fixed
* more work on memory management, including late-creation of Persistent handles (@rvagg)

## [0.4.3] - 2013-05-18

### Fixed
* better memory leak fix (@rvagg)

## [0.4.2] - 2013-05-17

### Fixed
* same memory leak fixes as 0.4.1, properly applied to batch() operations too (@rvagg)

## [0.4.1] - 2013-05-17

### Fixed
* fix memory leak caused when passing String objects in as keys and values, conversion to Slice created `new char[]` but wasn't being disposed. Buffers are automatically disposed (reported by @kylegetson levelup/#140) (@rvagg)

## [0.4.0] - 2013-05-15

### Changed
* upgrade to `LevelDB@1.10.0`, fairly minor changes, mostly bugfixes see https://groups.google.com/forum/#!topic/leveldb/O2Zdbi9Lrao for more info (@rvagg)

## [0.3.1] - 2013-05-14

### Fixed
* don't allow empty batch() operations through to LevelDB, on chained of array forms (@rvagg)

## [0.3.0] - 2013-05-14

### Added
* @No9 to contributors (@rvagg)

### Changed
* pull API tests up into `AbstractLevelDOWN`, require it to run the tests. `AbstractLevelDOWN` can now be used to test `LevelDOWN`-compliant API's (@maxogden)
* change iterator methods to return errors on the callbacks rather than throw (@mcollina & @rvagg)
* update documentation for `.get()` (@deanlandolt)

### Removed
* remove browserify shim (@rvagg)

### Fixed

**Historical Note** In the early days minor versions were looked upon as major versions. Semver practices we use today was not adopted fully at this time. This is why the history might look a bit confusing.

## 0.2.4 - 2013-05-21

**Historical Note** Did not publish as a tag and `package.json` was never committed with this version number. Most likely due to a locally modified `package.json`.

## [0.2.3] - 2013-05-17

### Fixed
* backport memory leak fixes (@rvagg)

## [0.2.2] - 2013-05-14

### Added
* node 0.10 to travis (@rvagg)
* @mcollina to contributors (@rvagg)
* browserify shim so `levelup` can run in the browser (@No9)

### Changed
* extract basic test code to `abstract-leveldown` (@maxogden)

### Removed

### Fixed

## [0.2.1] - 2013-04-08

### Changed
* ignore empty string/buffer start/end options on iterators (@kesla)
* macro cleanup, replace some with static inline functions (@rvagg)

### Fixed
* iterators: start on correct value when `reverse=true`, also handle end-of-store case #27 (@kesla)

## [0.2.0] - 2013-03-30

### Added
* windows support, using a combination of libuv and Windows-specific code. See README for details about what's required (@rvagg)
* `leveldown.destroy(location, cb)` to delete an existing `LevelDB` store, exposes `LevelDB.DestroyDB()` (@rvagg)
* `leveldown.repair(location, cb)` to repair an existing `LevelDB` store, exposes `LevelDB.RepairDB()` (@rvagg)
* advanced options: `writeBufferSize`, `blockSize`, `maxOpenFiles`, `blockRestartInterval`, exposes `LevelDB` options (@rvagg)
* chained batch operations. Argument-less `db.batch()` will return a new `Batch` object that can `.put()` and `.del()` and then `.write()`. API in flux so not documented yet. (@juliangruber / @rvagg)

### Changed
* auto-cleanup iterators that are left open when you close a database; any iterators left open when you close a database instance will kill your process so we now keep track of iterators and auto-close them before a db.close completes (@rvagg)

## [0.1.4] - 2013-03-11

### Changed
* return error when batch ops contain `null` or `undefined` (@rvagg / @ralphtheninja / @dominictarr)

## [0.1.3] - 2013-03-09

### Fixed
* `SmartOS` build problems (@wolfeidau)

## [0.1.2] - 2013-02-24

### Changed
* upgrade to `LevelDB@1.9.0`, fairly minor changes since 1.7.0 (@rvagg)
* upgrade to `Snappy@1.1.0`, changes block size to improve compression ~3%, slight decrease in speed (@rvagg)

## [0.1.1] - 2013-02-24

### Fixed
* compile error on Mac OS (@kesla / @rvagg)

## [0.1.0] - 2013-02-24

### Added
* add complete, independent test suite (@rvagg)

### Changed
* change API to export single function `levelup()` (@rvagg)
* move `createIterator()` to `levelup#iterator()` (@rvagg)
* make all `options` arguments optional (@rvagg)
* argument number & type checking on all methods (@rvagg)
* stricter checking on key & value types, `String`/`Object.toString()`/`Buffer`, non-zero-length (@rvagg)

### Removed
* remove `use namespace` and add `namespace leveldown` everywhere (@rvagg)

### Fixed
* race condition in Iterator `end()`/`next()` fix, merged from `LevelUP` (@ralphtheninja / @rvagg)

## [0.0.2] - 2013-01-20

### Changed
* finalise rename of internal components to `LevelDOWN`, removing `LevelUP` references (@rvagg)

## [0.0.1] - 2013-01-20

### Added
* complete documentation of current API (@rvagg)

### Fixed
* callback is not optional for `.close()` (@rvagg)

## 0.0.0 - 2013-01-06

:seedling: First release. Extracted from `levelup` as a stand-alone package (@rvagg)

[Unreleased]: https://github.com/level/leveldown/compare/v3.0.0...HEAD
[3.0.0]: https://github.com/level/leveldown/compare/v2.1.1...v3.0.0
[2.1.1]: https://github.com/level/leveldown/compare/v2.1.0...v2.1.1
[2.1.0]: https://github.com/level/leveldown/compare/v2.0.2...v2.1.0
[2.0.2]: https://github.com/level/leveldown/compare/v2.0.1...v2.0.2
[2.0.1]: https://github.com/level/leveldown/compare/v2.0.0...v2.0.1
[2.0.0]: https://github.com/level/leveldown/compare/v1.9.0...v2.0.0
[1.9.0]: https://github.com/level/leveldown/compare/v1.8.0...v1.9.0
[1.8.0]: https://github.com/level/leveldown/compare/v1.7.2...v1.8.0
[1.7.2]: https://github.com/level/leveldown/compare/v1.7.1...v1.7.2
[1.7.1]: https://github.com/level/leveldown/compare/v1.7.0...v1.7.1
[1.7.0]: https://github.com/level/leveldown/compare/v1.7.0-0...v1.7.0
[1.7.0-0]: https://github.com/level/leveldown/compare/v1.6.0...v1.7.0-0
[1.6.0]: https://github.com/level/leveldown/compare/v1.5.3...v1.6.0
[1.5.3]: https://github.com/level/leveldown/compare/v1.5.2...v1.5.3
[1.5.2]: https://github.com/level/leveldown/compare/v1.5.1...v1.5.2
[1.5.1]: https://github.com/level/leveldown/compare/v1.5.0...v1.5.1
[1.5.0]: https://github.com/level/leveldown/compare/v1.4.6...v1.5.0
[1.4.6]: https://github.com/level/leveldown/compare/v1.4.5...v1.4.6
[1.4.5]: https://github.com/level/leveldown/compare/v1.4.4...v1.4.5
[1.4.4]: https://github.com/level/leveldown/compare/v1.4.3...v1.4.4
[1.4.3]: https://github.com/level/leveldown/compare/v1.4.2...v1.4.3
[1.4.2]: https://github.com/level/leveldown/compare/v1.4.1...v1.4.2
[1.4.1]: https://github.com/level/leveldown/compare/v1.4.0...v1.4.1
[1.4.0]: https://github.com/level/leveldown/compare/v1.3.1-0...v1.4.0
[1.3.1-0]: https://github.com/level/leveldown/compare/v1.3.0...v1.3.1-0
[1.3.0]: https://github.com/level/leveldown/compare/v1.2.2...v1.3.0
[1.2.2]: https://github.com/level/leveldown/compare/v1.2.1...v1.2.2
[1.2.1]: https://github.com/level/leveldown/compare/v1.2.0...v1.2.1
[1.2.0]: https://github.com/level/leveldown/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/level/leveldown/compare/v1.0.7...v1.1.0
[1.0.7]: https://github.com/level/leveldown/compare/v1.0.6...v1.0.7
[1.0.6]: https://github.com/level/leveldown/compare/v1.0.5...v1.0.6
[1.0.5]: https://github.com/level/leveldown/compare/v1.0.4...v1.0.5
[1.0.4]: https://github.com/level/leveldown/compare/v1.0.3...v1.0.4
[1.0.3]: https://github.com/level/leveldown/compare/v1.0.2...v1.0.3
[1.0.2]: https://github.com/level/leveldown/compare/v1.0.1...v1.0.2
[1.0.1]: https://github.com/level/leveldown/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/level/leveldown/compare/v0.10.6...v1.0.0
[0.10.6]: https://github.com/level/leveldown/compare/v0.10.5...v0.10.6
[0.10.5]: https://github.com/level/leveldown/compare/v0.10.4...v0.10.5
[0.10.4]: https://github.com/level/leveldown/compare/v0.10.3...v0.10.4
[0.10.3]: https://github.com/level/leveldown/compare/v0.10.2...v0.10.3
[0.10.2]: https://github.com/level/leveldown/compare/v0.10.1...v0.10.2
[0.10.1]: https://github.com/level/leveldown/compare/0.10.0...v0.10.1
[0.10.0]: https://github.com/level/leveldown/compare/0.9.2...0.10.0
[0.9.2]: https://github.com/level/leveldown/compare/0.9.1...0.9.2
[0.9.1]: https://github.com/level/leveldown/compare/0.9.0...0.9.1
[0.9.0]: https://github.com/level/leveldown/compare/0.8.3...0.9.0
[0.8.3]: https://github.com/level/leveldown/compare/0.8.2...0.8.3
[0.8.2]: https://github.com/level/leveldown/compare/0.8.1...0.8.2
[0.8.1]: https://github.com/level/leveldown/compare/0.8.0...0.8.1
[0.8.0]: https://github.com/level/leveldown/compare/0.7.0...0.8.0
[0.7.0]: https://github.com/level/leveldown/compare/0.6.2...0.7.0
[0.6.2]: https://github.com/level/leveldown/compare/0.6.1...0.6.2
[0.6.1]: https://github.com/level/leveldown/compare/0.6.0...0.6.1
[0.6.0]: https://github.com/level/leveldown/compare/0.5.0...0.6.0
[0.5.0]: https://github.com/level/leveldown/compare/0.4.4...0.5.0
[0.4.4]: https://github.com/level/leveldown/compare/0.4.3...0.4.4
[0.4.3]: https://github.com/level/leveldown/compare/0.4.2...0.4.3
[0.4.2]: https://github.com/level/leveldown/compare/0.4.1...0.4.2
[0.4.1]: https://github.com/level/leveldown/compare/0.4.0...0.4.1
[0.4.0]: https://github.com/level/leveldown/compare/0.3.1...0.4.0
[0.3.1]: https://github.com/level/leveldown/compare/0.3.0...0.3.1
[0.3.0]: https://github.com/level/leveldown/compare/0.2.3...0.3.0
[0.2.3]: https://github.com/level/leveldown/compare/0.2.2...0.2.3
[0.2.2]: https://github.com/level/leveldown/compare/0.2.1...0.2.2
[0.2.1]: https://github.com/level/leveldown/compare/0.2.0...0.2.1
[0.2.0]: https://github.com/level/leveldown/compare/0.1.4...0.2.0
[0.1.4]: https://github.com/level/leveldown/compare/0.1.3...0.1.4
[0.1.3]: https://github.com/level/leveldown/compare/0.1.2...0.1.3
[0.1.2]: https://github.com/level/leveldown/compare/0.1.1...0.1.2
[0.1.1]: https://github.com/level/leveldown/compare/0.1.0...0.1.1
[0.1.0]: https://github.com/level/leveldown/compare/0.0.2...0.1.0
[0.0.2]: https://github.com/level/leveldown/compare/0.0.1...0.0.2
[0.0.1]: https://github.com/level/leveldown/compare/0.0.0...0.0.1

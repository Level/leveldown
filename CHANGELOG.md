# Changelog

## [Unreleased]

## [3.0.2] - 2018-05-05

### Changed
* Support compilation on iOS (@agentilela)
* Support compilation on Android x86 (@luandro)
* Upgrade to `prebuild-install@^4.0.0` (@ralphtheninja)

## [3.0.1] - 2018-04-26

### Added
* Run `verify-travis-appveyor` as part of tests to ensure they are in sync (@ralphtheninja)
* Test that `destroy()` doesn't inadvertently create the directory (@vweevers)
* Add node 10 to Travis and AppVeyor (@ralphtheninja)

### Changed
* Configure Greenkeeper to ignore updates to `@types/node` to reduce spam (@ralphtheninja)
* Upgrade to `nan@2.10` (@ralphtheninja)
* Use resource names in all `AsyncWorker` classes (@ralphtheninja)

### Fixed
* Handle all errors in destroy tests (@vweevers)
* Fix deprecation warnings related to `Buffer()` (@peakji)
* Fix deprecation warnings related to `nan` (@ralphtheninja)

### Removed
* Remove node 5 and 7 from AppVeyor (@ralphtheninja)
* Remove `console.log()` from `approximate-size-test.js` (@vweevers)

## [3.0.0] - 2018-01-30

### Changed
* Upgrade to `abstract-leveldown@4` (@ralphtheninja)
* Upgrade to `prebuild@7` (@ralphtheninja)
* Rewrite changelog (@ralphtheninja)

### Removed
* Remove node 5 and 7 from Travis (@ralphtheninja)

### Fixed
* Make sure `destroy()` deletes `LevelDB`-only dir (@joyeecheung)

## [2.1.1] - 2017-12-02

### Fixed
* Omit docs from `LevelDB` and test files from `Snappy` (@peakji)

## [2.1.0] - 2017-11-24

### Fixed
* Fix `Android` compilation (@staltz)

## [2.0.2] - 2017-11-23

### Added
* Add node 9 to Travis and AppVeyor (@ralphtheninja)

### Changed
* Upgrade to `nan@2.8` (@ralphtheninja)

## [2.0.1] - 2017-11-11

### Changed
* Major `README` overhaul (@ralphtheninja, @vweevers)
* Upgrade to `abstract-leveldown@3.0.0` (@vweevers)

### Fixed
* Tweak typings (@sandersn)

## [2.0.0] - 2017-10-02

### Changed
* Update `TypeScript` typings (@meirionhughes)

### Removed
* Remove support for node 0.12 (@ralphtheninja)

## [1.9.0] - 2017-09-28

### Added
* Add default export of `LevelDOWN` (@zixia)

## [1.8.0] - 2017-09-14

### Added
* Add node version badge (@vweevers)
* Add Greenkeeper badge (@ralphtheninja)
* Add `-mfloat-abi=hard` compiler flag for `arm` (@maihde)
* Add initial `TypeScript` typings (@meirionhughes)
* Add @meirionhughes to contributors (@meirionhughes)

### Changed
* Bump dependencies (@ralphtheninja)
* Simplify AppVeyor (@vweevers)
* Install latest `npm` on AppVeyor for `node@5` (@vweevers)

### Removed

### Fixed

## [1.7.2] - 2017-06-08

### Changed
* `iterator.next()` calls back with error if called after `iterator.end()` (@peakji)

### Fixed
* Closing db with open iterator should not crash (@peakji)

## [1.7.1] - 2017-06-01

### Added
* Add node 8 to Travis and AppVeyor (@ralphtheninja)

### Changed
* Skip installing latest `npm` on AppVeyor (@ralphtheninja)

## [1.7.0] - 2017-05-17

### Changed
* Bump version (@ralphtheninja)

## [1.7.0-0] - 2017-04-12

### Added
* Add support for `OpenBSD` (@qbit)

### Changed
* Upgrade to `LevelDB@1.20` (@peakji)
* Upgrade to `Snappy@1.1.4` (@peakji)
* Bump dependencies (@ralphtheninja)

## [1.6.0] - 2017-02-02

### Added
* Add `db.compactRange(start, end, cb)` (@bookchin)
* Add @bookchin to contributors (@bookchin)

### Changed
* Update `prebuild`, `prebuild-install` and `prebuild-ci` (@juliangruber)
* Update `nan` (@ralphtheninja)

## [1.5.3] - 2016-12-30

### Added
* Restore node 5 to AppVeyor (@vweevers)

## [1.5.2] - 2016-12-29

### Added
* Restore node 5 to Travis (@ralphtheninja)

## [1.5.1] - 2016-12-27

### Added
* Add node 7 to Travis (@ralphtheninja)
* Add AppVeyor so we can build on Windows (@vweevers)
* Add OSX to Travis (@juliangruber)
* Add `prebuild-ci` so we can automatically make prebuilt binaries (@juliangruber)

### Changed
* Enable `iterator.seek()` with buffers (@peakji)

### Removed
* Remove node 0.10 from Travis (@ralphtheninja)
* Remove node 5 from Travis (@ralphtheninja)

## [1.5.0] - 2016-09-27

### Changed
* Upgrade to `LevelDB@1.19` (@juliangruber)
* Bump dependencies (@ralphtheninja)
* Upgrade to `nan@2.4.0`, remove `Function::NewInstance()` deprecation warnings (@rvagg)

### Removed
* Remove deleted file from `leveldb.gyp` (@juliangruber)

## [1.4.6] - 2016-04-29

### Added
* Add node 6 to Travis (@ralphtheninja)

### Changed
* Upgrade to `nan@2.3.0` to remove deprecated calls (@ralphtheninja)

## [1.4.5] - 2016-04-18

### Added
* Add @chjj's script for checking memory consumption (@ralphtheninja)

### Changed
* Bump dependencies (@ralphtheninja)
* Ignore Release folder for Windows (@vweevers)
* Update copyright year 2015 -> 2016 (@ralphtheninja)
* Optimize iterators by avoiding handles (@chjj)

### Removed
* Remove unsupported versions from Travis (@ralphtheninja)

### Fixed
* Fix delete calls not using `[]` (@chjj)
* Free start slices on conflicting options (@chjj)
* Dispose of unused slices (@chjj)
* Release iterator snapshots (@chjj)
* Fix iterator leak (@chjj)
* Add handlescopes to fix potential memory leaks (@chjj)
* Fix repair-test for multilang windows (@vweevers)
* Repair error is different on windows (@ralphtheninja)

## [1.4.4] - 2016-01-25

### Changed
* Update dependencies (@ralphtheninja)
* Use `prebuild --install` (@ralphtheninja)
* `README` fixes (@ralphtheninja)

### Fixed
* Fix build problems on windows https://github.com/Level/leveldown/issues/247 (@obastemur)

## [1.4.3] - 2015-12-14

### Added
* Add node 5 to Travis (@ralphtheninja)
* Add dependency badge (@ralphtheninja)

### Changed
* Bump dependencies (@ralphtheninja)
* Bump `prebuild` for `--all` functionality (@ralphtheninja)
* Add node 5 to prebuild config (abi 47) (@ralphtheninja)

### Fixed
* Fix build problems on windows https://github.com/Level/leveldown/issues/196 (@obastemur)

## [1.4.2] - 2015-10-21

### Added
* Add node 4 to prebuild config (abi 46) (@ralphtheninja)
* Add node 4 to Travis (@ralphtheninja)

### Changed
* Test on latest node versions (@ralphtheninja)
* Document `prebuild` and explain different compile steps (@ralphtheninja)
* Update prebuild for --debug and --strip support (@ralphtheninja)
* Require `fast-future` at start, not later (@timkuijsten)
* Add @kkoopa's Travis fix (@mafintosh)

## [1.4.1] - 2015-08-15

### Added
* Add abi 45 to `prebuild` (@mafintosh)
* Add io.js 3.0 to Travis (@mafintosh)

### Changed
* Update `prebuild` (@ralphtheninja)
* Replace missing `Handle<>` with `Local<>` (@mafintosh)
* Upgrade to `nan@2` (@rvagg)

## [1.4.0] - 2015-07-28

### Added
* Add `.prebuildrc` update Travis and prebuild, add prebuild npm script (@ralphtheninja)
* Add rebuild npm script (@ralphtheninja)

### Changed
* Switch from `node-pre-gyp` to `prebuild` (@ralphtheninja)

### Removed
* No longer need `node-gyp` directly (@ralphtheninja)

### Fixed

## [1.3.1-0] - 2015-07-20

### Changed
* `node-pre-gyp` should build binaries on Travis (@ralphtheninja)

### Removed
* Remove `hash_test.cc` source (@michaelnisi)

## [1.3.0] - 2015-06-16

### Added
* Add io.js 1.0 to Travis (@ralphtheninja)

### Changed
* Upgrade to `LevelDB@1.18.0` (Braydon Fuller)

### Removed
* Remove io.js 2.0 from Travis (@ralphtheninja)
* Remove io.js 2.1 from Travis (@ralphtheninja)

### Fixed
* Add `LEVELDB_PLATFORM_UV` to `LevelDB` `port.h` to fix test (Braydon Fuller)

## [1.2.2] - 2015-06-02

### Fixed
* Ignore `build-pre-gyp/` folder when publishing to npm (@ralphtheninja)

## [1.2.1] - 2015-06-01

### Changed
* Use `remote_path` with `node-pre-gyp` to dodge preparing `package.json` every time (@ralphtheninja)
* Add more io.js versions (@ralphtheninja)
* Use `node-gyp-install` to make sure correct `node-gyp` headers are downloaded (@ralphtheninja)

## [1.2.0] - 2015-06-01

### Added
* Add `iterator.seek()` (@mafintosh)

## [1.1.0] - 2015-05-28

### Changed
* Upgrade `abstract-leveldown`, mainly for `.status` (@juliangruber)

## [1.0.7] - 2015-05-27

### Added
* Add compression test suite (@juliangruber)
* Add link to `level/community` (@ralphtheninja)

### Changed
* Use const reference instead of by value in `Database` constructor (@ralphtheninja)
* Refactor `NanNew()` on strings into option value functions (@ralphtheninja)
* Refactor `BooleanOptionValue` (@ralphtheninja)
* `NanUInt32OptionValue` -> `UInt32OptionValue` (@ralphtheninja)
* `NanBooleanOptionValue` -> `BooleanOptionValue` (@ralphtheninja)
* Simplify location logic, let `Database` take care of allocation (@ralphtheninja)
* Update `abstract-leveldown` (@ralphtheninja)
* Update `.travis.yml`, nvm works on Travis now (@ralphtheninja)

## [1.0.6] - 2015-05-05

### Changed
* Bump version (@ralphtheninja)

## [1.0.5] - 2015-05-05

### Fixed
* Pass db to `AbstractIterator` so gc keeps it (@juliangruber)

## [1.0.4] - 2015-05-05

### Changed
* Update `nan` for iojs 2.0.0 (@ralphtheninja)

## [1.0.3] - 2015-05-02

### Changed
* `tap` -> `tape` + `faucet` (@ralphtheninja)

### Fixed
* Fix `write-random.js`, use `leveldown` instead of `lmdb` (@ralphtheninja)
* Fix `bench/db-bench.js` (@ralphtheninja)

## [1.0.2] - 2015-04-26

### Added
* Add documentation about snapshots (@maxogden)

### Changed
* Update logo and copyright (@ralphtheninja)
* s/rvagg\/node-/level\// (@ralphtheninja)
* Use `n` instead of `nvm` for working `io.js` support (@ralphtheninja)
* Update to `abstract-leveldown@2.1.0` (@ralphtheninja)

## [1.0.1] - 2015-01-16

### Changed
* Upgrade to `nan@1.5` for `io.js` support (@rvagg)

### Fixed
* Fix `LevelDB` builds for modern gcc versions (@sharvil)

## [1.0.0] - 2014-08-26

### Changed
* Upgrade to `nan@1.3` for Node 0.11.13+ support (@rvagg)
* Upgrade to `LevelDB@1.17.0` (@kesla)
* Allow writing empty values: null, undefined, '', [] and Buffer(0). Entries come out as '' or Buffer(0) (@ggreer, @juliangruber, @rvagg)
* Massive speed up of iterators by chunking reads (@kesla)
* Wrap in abstract-leveldown for consistent type-checking across \*DOWNs (@kesla)
* Switch to plain MIT license (@andrewrk)

### Removed
* Remove compile option that borked EL5 compiles (@rvagg)

### Fixed
* Fix clang build (@thlorenz)
* Fix minor memory leaks in options attributes (@ggreer)

## [0.10.6] - 2016-01-07

### Added
* Add iojs, node 4 and 5 to Travis (@ralphtheninja)

### Changed
* Upgrade to `nan@2.1.x` for `node@4` and `node@5` support (@ralphtheninja)

### Removed
* Remove `node@0.8` from Travis (@ralphtheninja)

## [0.10.5] - 2015-05-05

### Changed
* Upgrade to `nan@1.8.x` for `iojs` support (@ralphtheninja)

## [0.10.4] - 2015-02-07

### Changed
* Bump version (@rvagg)

## [0.10.3] - 2015-02-07

### Changed
* Upgrade to `nan@1.5` (@mcollina)

## [0.10.2] - 2013-11-30

### Fixed
* Apply fix by @rescrv for long-standing [OSX corruption bug](https://groups.google.com/forum/#!topic/leveldb/GXhx8YvFiig) (@rvagg / @rescrv)

## [0.10.1] - 2013-11-21

### Changed
* Upgrade to `nan@0.6` for Node@0.11.6 support, `v8::Local<T>::New(val)` rewritten to `NanNewLocal<T>(val)` (@rvagg)

**Historical Note** From this release and onward, tags in git start with the prefix `v`, i.e. this release corresponds to the tag `v0.10.1`.

## [0.10.0] - 2013-11-18

### Added
* Add new simple batch() leak tester (@rvagg)

### Changed
* Upgrade to `Snappy@1.1.1` (@rvagg and @No9)
* Upgrade to `nan@0.5.x` (@rvagg)
* Upgrade to `abstract-leveldown@0.11.x` for testing (@rvagg)
* Switch all `callback->Call()`s to `node::MakeCallback()` to properly support Node.js domains (@rvagg)
* Enable LevelDB's BloomFilter (@Kyotoweb)
* Properly enable compression by default (@Kyotoweb)

### Removed
* Remove `Persistent` references for all `batch()` operations as `WriteBatch` takes an explicit copy of the data (@mcollina and @rvagg)

### Fixed
* Fix array-batch memory leak, levelup/#171 (@rvagg)
* Fix chained-batch `write()` segfaults, details in #73, (@rvagg and @mcollina)

## [0.9.2] - 2013-11-02

### Fixed
* Minor fixes to support `Node@0.11.8` and new Linux gcc (warnings) (@rvagg)

## [0.9.1] - 2013-10-03

### Fixed
* Include `port_uv.h` for Windows compile, added test to suite to make sure this happens every time `LevelDB` is upgraded (@rvagg)

## [0.9.0] - 2013-10-01

### Changed
* Upgrade to `LevelDB@0.14.0`, includes change from .sst to .ldb file extension for SST files (@rvagg)

## [0.8.3] - 2013-09-18

### Changed
* Upgrade to `nan@0.4.0`, better support for latest Node master & support for installing within directory structures containing spaces in directory names (@rvagg)

**Historical Note** The diff between this version and the previous shows `0.8.4` in the commit message. This is incorrect, since that version was never released.

## [0.8.2] - 2013-09-01

### Added
* Add support for `FreeBSD` (@rvagg, @kelexel)

## [0.8.1] - 2013-09-01

### Added
* Add @substack to contributors (@rvagg)

### Fixed
* Fix minor V8-level leaks (@rvagg)

## [0.8.0] - 2013-08-26

### Added
* Add `gt`, `lt`, `gte`, `lte` for iterators (@dominictarr)

### Changed
* Switch to `nan` as an npm dependency (@rvagg)

## [0.7.0] - 2013-08-11

### Added
* Add @pgte to contributors (@rvagg)

### Changed
* Use [nan](https://github.com/rvagg/nan) for Node 0.8->0.11 compatibility (@rvagg)
* Minor perf improvements in C++ (@mscdex)

**Historical Note** We started using `nan` in the form of a local `src/nan.h` header file.

## [0.6.2] - 2013-07-07

### Changed
* Compatibility for `Node@0.11.3`, breaks compatibility with 0.11.2

## [0.6.1] - 2013-06-15

### Fixed
* Fix broken Windows compile, apply port-uv patch to `LevelDB`'s port.h (@rvagg)

## [0.6.0] - 2013-06-14

### Changed
* Upgrade to `LevelDB@1.11.0`, some [important bugfixes](https://groups.google.com/forum/#!topic/leveldb/vS1JvmGlp4E) (@rvagg)

## [0.5.0] - 2013-05-21

### Changed
* Bump major version for `db.getProperty()` addition (should have been done in 0.4.4) (@rvagg)
* Disallow `batch()` operations after a `write()` (@rvagg)

## [0.4.4] - 2013-05-18

### Added
* Add `db.getProperty()`, see README for details (@rvagg)

### Fixed
* More work on memory management, including late-creation of Persistent handles (@rvagg)

## [0.4.3] - 2013-05-18

### Fixed
* Better memory leak fix (@rvagg)

## [0.4.2] - 2013-05-17

### Fixed
* Same memory leak fixes as 0.4.1, properly applied to batch() operations too (@rvagg)

## [0.4.1] - 2013-05-17

### Fixed
* Fix memory leak caused when passing String objects in as keys and values, conversion to Slice created `new char[]` but wasn't being disposed. Buffers are automatically disposed (reported by @kylegetson levelup/#140) (@rvagg)

## [0.4.0] - 2013-05-15

### Changed
* Upgrade to `LevelDB@1.10.0`, fairly minor changes, mostly bugfixes see https://groups.google.com/forum/#!topic/leveldb/O2Zdbi9Lrao for more info (@rvagg)

## [0.3.1] - 2013-05-14

### Fixed
* Don't allow empty batch() operations through to LevelDB, on chained of array forms (@rvagg)

## [0.3.0] - 2013-05-14

### Added
* Add @No9 to contributors (@rvagg)

### Changed
* Pull API tests up into `AbstractLevelDOWN`, require it to run the tests. `AbstractLevelDOWN` can now be used to test `LevelDOWN`-compliant API's (@maxogden)
* Change iterator methods to return errors on the callbacks rather than throw (@mcollina & @rvagg)
* Update documentation for `.get()` (@deanlandolt)

### Removed
* Remove browserify shim (@rvagg)

**Historical Note** In the early days minor versions were looked upon as major versions. Semver practices we use today was not adopted fully at this time. This is why the history might look a bit confusing.

## 0.2.4 - 2013-05-21

**Historical Note** Did not publish as a tag and `package.json` was never committed with this version number. Most likely due to a locally modified `package.json`.

## [0.2.3] - 2013-05-17

### Fixed
* Backport memory leak fixes (@rvagg)

## [0.2.2] - 2013-05-14

### Added
* Add node 0.10 to Travis (@rvagg)
* Add @mcollina to contributors (@rvagg)
* Add browserify shim so `levelup` can run in the browser (@No9)

### Changed
* Extract basic test code to `abstract-leveldown` (@maxogden)

## [0.2.1] - 2013-04-08

### Changed
* Ignore empty string/buffer start/end options on iterators (@kesla)
* Macro cleanup, replace some with static inline functions (@rvagg)

### Fixed
* Fix iterator to start on correct value when `reverse=true`, also handle end-of-store case #27 (@kesla)

## [0.2.0] - 2013-03-30

### Added
* Add windows support, using a combination of libuv and Windows-specific code. See README for details about what's required (@rvagg)
* Add `leveldown.destroy(location, cb)` to delete an existing `LevelDB` store, exposes `LevelDB.DestroyDB()` (@rvagg)
* Add `leveldown.repair(location, cb)` to repair an existing `LevelDB` store, exposes `LevelDB.RepairDB()` (@rvagg)
* Add advanced options: `writeBufferSize`, `blockSize`, `maxOpenFiles`, `blockRestartInterval`, exposes `LevelDB` options (@rvagg)
* Add chained batch operations. Argument-less `db.batch()` will return a new `Batch` object that can `.put()` and `.del()` and then `.write()`. API in flux so not documented yet. (@juliangruber / @rvagg)

### Changed
* Auto-cleanup iterators that are left open when you close a database; any iterators left open when you close a database instance will kill your process so we now keep track of iterators and auto-close them before a db.close completes (@rvagg)

## [0.1.4] - 2013-03-11

### Changed
* Return error when batch ops contain `null` or `undefined` (@rvagg / @ralphtheninja / @dominictarr)

## [0.1.3] - 2013-03-09

### Fixed
* `SmartOS` build problems (@wolfeidau)

## [0.1.2] - 2013-02-24

### Changed
* Upgrade to `LevelDB@1.9.0`, fairly minor changes since 1.7.0 (@rvagg)
* Upgrade to `Snappy@1.1.0`, changes block size to improve compression ~3%, slight decrease in speed (@rvagg)

## [0.1.1] - 2013-02-24

### Fixed
* Compile error on Mac OS (@kesla / @rvagg)

## [0.1.0] - 2013-02-24

### Added
* Add complete, independent test suite (@rvagg)

### Changed
* Change API to export single function `levelup()` (@rvagg)
* Move `createIterator()` to `levelup#iterator()` (@rvagg)
* Make all `options` arguments optional (@rvagg)
* Argument number & type checking on all methods (@rvagg)
* Stricter checking on key & value types, `String`/`Object.toString()`/`Buffer`, non-zero-length (@rvagg)

### Removed
* Remove `use namespace` and add `namespace leveldown` everywhere (@rvagg)

### Fixed
* Fix race condition in Iterator `end()`/`next()`, merged from `LevelUP` (@ralphtheninja / @rvagg)

## [0.0.2] - 2013-01-20

### Changed
* Finalize rename of internal components to `LevelDOWN`, removing `LevelUP` references (@rvagg)

## [0.0.1] - 2013-01-20

### Added
* Complete documentation of current API (@rvagg)

### Fixed
* Callback is not optional for `.close()` (@rvagg)

## 0.0.0 - 2013-01-06

:seedling: First release. Extracted from `levelup` as a stand-alone package (@rvagg)

[Unreleased]: https://github.com/level/leveldown/compare/v3.0.2...HEAD
[3.0.2]: https://github.com/level/leveldown/compare/v3.0.1...v3.0.2
[3.0.1]: https://github.com/level/leveldown/compare/v3.0.0...v3.0.1
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

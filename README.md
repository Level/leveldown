LevelDOWN
=========

A Low-level Node.js LevelDB binding
-------------------------

[![Build Status](https://secure.travis-ci.org/rvagg/node-leveldown.png)](http://travis-ci.org/rvagg/node-leveldown)

LevelDOWN was extracted from [LevelUP](https://github.com/rvagg/node-levelup) and now serves as a stand-alone binding for LevelDB.

It is **strongly recommended** that you use LevelUP in preference to LevelDOWN unless you have measurable performance reasons to do so. LevelUP is optimised for usability and safety. Although we are working to improve the safety of the LevelDOWN interface it is still easy to crash your Node process if you don't do things in just the right way.

See the section on <a href="#safety">safety</a> below for details of known unsafe operations with LevelDOWN.

<a name="api"></a>
## API

  * <a href="#ctor"><code><b>leveldown()</b></code></a>
  * <a href="#leveldown_open"><code><b>leveldown#open()</b></code></a>
  * <a href="#leveldown_close"><code><b>leveldown#close()</b></code></a>
  * <a href="#leveldown_put"><code><b>leveldown#put()</b></code></a>
  * <a href="#leveldown_get"><code><b>leveldown#get()</b></code></a>
  * <a href="#leveldown_del"><code><b>leveldown#del()</b></code></a>
  * <a href="#leveldown_batch"><code><b>leveldown#batch()</b></code></a>
  * <a href="#leveldown_approximateSize"><code><b>leveldown#approximateSize()</b></code></a>
  * <a href="#leveldown_iterator"><code><b>leveldown#iterator()</b></code></a>
  * <a href="#iterator_next"><code><b>iterator#next()</b></code></a>
  * <a href="#iterator_end"><code><b>iterator#end()</b></code></a>


--------------------------------------------------------
<a name="ctor"></a>
### leveldown(location)
<code>leveldown()</code> returns a new **LevelDOWN** instance. `location` is a String pointing to the LevelDB location to be opened.


--------------------------------------------------------
<a name="leveldown_open"></a>
### leveldown#open([options, ]callback)
<code>open()</code> is an instance method on an existing database object.

#### `options`

The optional `options` argument may contain:

* `'createIfMissing'` *(boolean, default: `true`)*: If `true`, will initialise an empty database at the specified location if one doesn't already exist. If `false` and a database doesn't exist you will receive an error in your `open()` callback and your database won't open.

* `'errorIfExists'` *(boolean, default: `false`)*: If `true`, you will receive an error in your `open()` callback if the database exists at the specified location.

* `'compression'` *(boolean, default: `true`)*: If `true`, all *compressible* data will be run through the Snappy compression algorithm before being stored. Snappy is very fast and shouldn't gain much speed by disabling so leave this on unless you have good reason to turn it off.

* `'cacheSize'` *(number, default: `8 * 1024 * 1024`)*: The size (in bytes) of the in-memory [LRU](http://en.wikipedia.org/wiki/Cache_algorithms#Least_Recently_Used) cache with frequently used uncompressed block contents. 

The `callback` function will be called with no arguments when the database has been successfully opened, or with a single `error` argument if the open operation failed for any reason.


--------------------------------------------------------
<a name="leveldown_close"></a>
### leveldown#close(callback)
<code>close()</code> is an instance method on an existing database object. The underlying LevelDB database will be closed and the `callback` function will be called with no arguments if the operation is successful or with a single `error` argument if the operation failed for any reason.


--------------------------------------------------------
<a name="leveldown_put"></a>
### leveldown#put(key, value[, options], callback)
<code>put()</code> is an instance method on an existing database object, used to store new entries, or overwrite existing entries in the LevelDB store.

The `key` and `value` objects may either be `String`s or Node.js `Buffer` objects and cannot be `undefined` or `null`. Other object types are converted to JavaScript `String`s with the `toString()` method and the resulting `String` *may not* be a zero-length. A richer set of data-types are catered for in LevelUP.

#### `options`

The only property currently available on the `options` object is `'sync'` *(boolean, default: `false`)*. If you provide a `'sync'` value of `true` in your `options` object, LevelDB will perform a synchronous write of the data; although the operation will be asynchronous as far as Node is concerned. Normally, LevelDB passes the data to the operating system for writing and returns immediately, however a synchronous write will use `fsync()` or equivalent so your callback won't be triggered until the data is actually on disk. Synchronous filesystem writes are **significantly** slower than asynchronous writes but if you want to be absolutely sure that the data is flushed then you can use `'sync': true`.

The `callback` function will be called with no arguments if the operation is successful or with a single `error` argument if the operation failed for any reason.


--------------------------------------------------------
<a name="leveldown_get"></a>
### leveldown#get(key[, options], callback)
<code>get()</code> is an instance method on an existing database object, used to fetch individual entries from the LevelDB store.

The `key` object may either be a `String` or a Node.js `Buffer` object and cannot be `undefined` or `null`. Other object types are converted to JavaScript `String`s with the `toString()` method and the resulting `String` *may not* be a zero-length. A richer set of data-types are catered for in LevelUP.

#### `options`

The optional `options` object may contain:

* `'fillCache'` *(boolean, default: `true`)*: LevelDB will by default fill the in-memory LRU Cache with data from a call to get. Disabling this is done by setting `fillCache` to `false`.

* `'asBuffer'` *(boolean, default: `true`)*: Used to determine whether to return the `value` of the entry as a `String` or a Node.js `Buffer` object. Note that converting from a `Buffer` to a `String` incurs a cost so if you need a `String` (and the `value` can legitimately become a UFT8 string) then you should fetch it as one with `asBuffer: true` and you'll avoid this conversion cost.

The `callback` function will be called with no arguments if the operation is successful or with a single `error` argument if the operation failed for any reason.


--------------------------------------------------------
<a name="leveldown_del"></a>
### leveldown#del(key[, options], callback)
<code>del()</code> is an instance method on an existing database object, used to delete entries from the LevelDB store.

The `key` object may either be a `String` or a Node.js `Buffer` object and cannot be `undefined` or `null`. Other object types are converted to JavaScript `String`s with the `toString()` method and the resulting `String` *may not* be a zero-length. A richer set of data-types are catered for in LevelUP.

#### `options`

The only property currently available on the `options` object is `'sync'` *(boolean, default: `false`)*. See <a href="#leveldown_put">leveldown#put()</a> for details about this option.

The `callback` function will be called with no arguments if the operation is successful or with a single `error` argument if the operation failed for any reason.


--------------------------------------------------------
<a name="leveldown_batch"></a>
### leveldown#batch(operations[, options], callback)
<code>batch()</code> is an instance method on an existing database object. Used for very fast bulk-write operations (both *put* and *delete*). The `operations` argument should be an `Array` containing a list of operations to be executed sequentially. Each operation is contained in an object having the following properties: `type`, `key`, `value`, where the *type* is either `'put'` or `'del'`. In the case of `'del'` the `'value'` property is ignored. See [LevelUP](https://github.com/rvagg/node-levelup#batch) for full documentation on how this works in practice.

#### `options`

The only property currently available on the `options` object is `'sync'` *(boolean, default: `false`)*. See <a href="#leveldown_put">leveldown#put()</a> for details about this option.

The `callback` function will be called with no arguments if the operation is successful or with a single `error` argument if the operation failed for any reason.


--------------------------------------------------------
<a name="leveldown_approximateSize"></a>
### leveldown#approximateSize(start, end, callback)
<code>approximateSize()</code> is an instance method on an existing database object. Used to get the approximate number of bytes of file system space used by the range `[start..end)`. The result may not include recently written data.

The `start` and `end` parameters may be either `String` or Node.js `Buffer` objects representing keys in the LevelDB store.

The `callback` function will be called with no arguments if the operation is successful or with a single `error` argument if the operation failed for any reason.


--------------------------------------------------------
<a name="createIterator"></a>
### leveldown#iterator([options])
<code>iterator()</code> is an instance method on an existing database object. It returns a new **Iterator** instance.

#### `options`

The optional `options` object may contain:

* `'start'`: the key you wish to start the read at. By default it will start at the beginning of the store. Note that the *start* doesn't have to be an actual key that exists, LevelDB will simply find the *next* key, greater than the key you provide.

* `'end'`: the key you wish to end the read on. By default it will continue until the end of the store. Again, the *end* doesn't have to be an actual key as an (inclusive) `<=`-type operation is performed to detect the end. You can also use the `destroy()` method instead of supplying an `'end'` parameter to achieve the same effect.

* `'reverse'` *(boolean, default: `false`)*: a boolean, set to true if you want the stream to go in reverse order. Beware that due to the way LevelDB works, a reverse seek will be slower than a forward seek.

* `'keys'` *(boolean, default: `true`)*: whether the callback to the `next()` method should receive a non-null `key`. There is a small efficiency gain if you ultimately don't care what the keys are as they don't need to be converted and copied into JavaScript.

* `'values'` *(boolean, default: `true`)*: whether the callback to the `next()` method should receive a non-null `value`. There is a small efficiency gain if you ultimately don't care what the values are as they don't need to be converted and copied into JavaScript.

* `'limit'` *(number, default: `-1`)*: limit the number of results collected by this iterator. This number represents a *maximum* number of results and may not be reached if you get to the end of the store or your `'end'` value first. A value of `-1` means there is no limit.

* `'fillCache'` *(boolean, default: `false`)*: wheather LevelDB's LRU-cache should be filled with data read.

* `'keyAsBuffer'` *(boolean, default: `true`)*: Used to determine whether to return the `key` of each entry as a `String` or a Node.js `Buffer` object. Note that converting from a `Buffer` to a `String` incurs a cost so if you need a `String` (and the `value` can legitimately become a UFT8 string) then you should fetch it as one.

* `'valueAsBuffer'` *(boolean, default: `true`)*: Used to determine whether to return the `value` of each entry as a `String` or a Node.js `Buffer` object.


--------------------------------------------------------
<a name="iterator_next"></a>
### iterator#next(callback)
<code>next()</code> is an instance method on an existing iterator object, used to increment the underlying LevelDB iterator and return the entry at that location.

the `callback` function will be called with no arguments in any of the following situations:

* the iterator comes to the end of the store
* the `end` key has been reached; or
* the `limit` has been reached

Otherwise, the `callback` function will be called with the following 3 arguments:

* `error` - any error that occurs while incrementing the iterator.
* `key` - either a `String` or a Node.js `Buffer` object depending on the `keyAsBuffer` argument when the `createIterator()` was called.
* `value` - either a `String` or a Node.js `Buffer` object depending on the `valueAsBuffer` argument when the `createIterator()` was called.


--------------------------------------------------------
<a name="iterator_end"></a>
### iterator#end(callback)
<code>end()</code> is an instance method on an existing iterator object. The underlying LevelDB iterator will be deleted and the `callback` function will be called with no arguments if the operation is successful or with a single `error` argument if the operation failed for any reason.


<a name="safety"></a>
Safety
------

### Database state

Currently LevelDOWN does not track the state of the underlying LevelDB instance. This means that calling `open()` on an already open database may result in an error. Likewise, calling any other operation on a non-open database may result in an error.

LevelUP currently tracks and manages state and will prevent out-of-state operations from being send to LevelDOWN. If you use LevelDOWN directly then you must track and manage state.

### Iterators

You should make sure to **always** `end()` your LevelDOWN Iterators, it will not be done for you, even on a `close()` of the database your Iterator belongs to. If you don't `end()` then the underlying resources aren't cleaned up and this will result in a Node process death.


<a name="contributing"></a>
Contributing
------------

LevelDOWN is an **OPEN Open Source Project**. This means that:

> Individuals making significant and valuable contributions are given commit-access to the project to contribute as they see fit. This project is more like an open wiki than a standard guarded open source project.

See the [CONTRIBUTING.md](https://github.com/rvagg/node-leveldown/blob/master/CONTRIBUTING.md) file for more details.

### Contributors

LevelDOWN is only possible due to the excellent work of the following contributors:

<table><tbody>
<tr><th align="left">Rod Vagg</th><td><a href="https://github.com/rvagg">GitHub/rvagg</a></td><td><a href="http://twitter.com/rvagg">Twitter/@rvagg</a></td></tr>
<tr><th align="left">John Chesley</th><td><a href="https://github.com/chesles/">GitHub/chesles</a></td><td><a href="http://twitter.com/chesles">Twitter/@chesles</a></td></tr>
<tr><th align="left">Jake Verbaten</th><td><a href="https://github.com/raynos">GitHub/raynos</a></td><td><a href="http://twitter.com/raynos2">Twitter/@raynos2</a></td></tr>
<tr><th align="left">Dominic Tarr</th><td><a href="https://github.com/dominictarr">GitHub/dominictarr</a></td><td><a href="http://twitter.com/dominictarr">Twitter/@dominictarr</a></td></tr>
<tr><th align="left">Max Ogden</th><td><a href="https://github.com/maxogden">GitHub/maxogden</a></td><td><a href="http://twitter.com/maxogden">Twitter/@maxogden</a></td></tr>
<tr><th align="left">Lars-Magnus Skog</th><td><a href="https://github.com/ralphtheninja">GitHub/ralphtheninja</a></td><td><a href="http://twitter.com/ralphtheninja">Twitter/@ralphtheninja</a></td></tr>
<tr><th align="left">David Björklund</th><td><a href="https://github.com/kesla">GitHub/kesla</a></td><td><a href="http://twitter.com/david_bjorklund">Twitter/@david_bjorklund</a></td></tr>
<tr><th align="left">Julian Gruber</th><td><a href="https://github.com/juliangruber">GitHub/juliangruber</a></td><td><a href="http://twitter.com/juliangruber">Twitter/@juliangruber</a></td></tr>
<tr><th align="left">Paolo Fragomeni</th><td><a href="https://github.com/hij1nx">GitHub/hij1nx</a></td><td><a href="http://twitter.com/hij1nx">Twitter/@hij1nx</a></td></tr>
</tbody></table>

<a name="licence"></a>
Licence &amp; copyright
-------------------

Copyright (c) 2012-2013 LevelDOWN contributors (listed above).

LevelDOWN is licensed under an MIT +no-false-attribs license. All rights not explicitly granted in the MIT license are reserved. See the included LICENSE file for more details.

*LevelDOWN builds on the excellent work of the LevelDB and Snappy teams from Google and additional contributors. LevelDB and Snappy are both issued under the [New BSD Licence](http://opensource.org/licenses/BSD-3-Clause).*
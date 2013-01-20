LevelDOWN
=========

A Node.js LevelDB binding (currently being extracted from LevelUP)
-------------------------

<a name="api"></a>
## API

--------------------------------------------------------
<a name="createDatabase"></a>
### leveldown.createDatabase()
<code>createDatabase()</code> returns a new **LevelDOWN** instance.

--------------------------------------------------------
<a name="createIterator"></a>
### leveldown.createIterator(database, options)
<code>createIterator()</code> returns a new **Iterator** instance for the given database instance.

The `options` object may contain:

* `'start'`: the key you wish to start the read at. By default it will start at the beginning of the store. Note that the *start* doesn't have to be an actual key that exists, LevelDB will simply find the *next* key, greater than the key you provide.

* `'end'`: the key you wish to end the read on. By default it will continue until the end of the store. Again, the *end* doesn't have to be an actual key as an (inclusive) `<=`-type operation is performed to detect the end. You can also use the `destroy()` method instead of supplying an `'end'` parameter to achieve the same effect.

* `'reverse'` *(boolean, default: `false`)*: a boolean, set to true if you want the stream to go in reverse order. Beware that due to the way LevelDB works, a reverse seek will be slower than a forward seek.

* `'keys'` *(boolean, default: `true`)*: whether the `'data'` event should contain keys. If set to `true` and `'values'` set to `false` then `'data'` events will simply be keys, rather than objects with a `'key'` property. Used internally by the `keyStream()` method.

* `'values'` *(boolean, default: `true`)*: whether the `'data'` event should contain values. If set to `true` and `'keys'` set to `false` then `'data'` events will simply be values, rather than objects with a `'value'` property. Used internally by the `valueStream()` method.

* `'limit'` *(number, default: `-1`)*: limit the number of results collected by this stream. This number represents a *maximum* number of results and may not be reached if you get to the end of the store or your `'end'` value first. A value of `-1` means there is no limit.

* `'fillCache'` *(boolean, default: `false`)*: wheather LevelDB's LRU-cache should be filled with data read.


--------------------------------------------------------
<a name="leveldown_open"></a>
### leveldown#open(location, options, callback)
<code>open()</code> is an instance method on an existing database object. `location` is a String pointing to the LevelDB location to be opened.

The `options` object may contain:

* `'createIfMissing'` *(boolean, default: `true`)*: If `true`, will initialise an empty database at the specified location if one doesn't already exist. If `false` and a database doesn't exist you will receive an error in your `open()` callback and your database won't open.

* `'errorIfExists'` *(boolean, default: `false`)*: If `true`, you will receive an error in your `open()` callback if the database exists at the specified location.

* `'compression'` *(boolean, default: `true`)*: If `true`, all *compressible* data will be run through the Snappy compression algorithm before being stored. Snappy is very fast and shouldn't gain much speed by disabling so leave this on unless you have good reason to turn it off.

* `'cacheSize'` *(number, default: `8 * 1024 * 1024`)*: The size (in bytes) of the in-memory [LRU](http://en.wikipedia.org/wiki/Cache_algorithms#Least_Recently_Used) cache with frequently used uncompressed block contents. 

The `callback` function will be called with no arguments when the database has been successfully opened, or with a single, `error` argument if the open operation failed for any reason.

--------------------------------------------------------
<a name="leveldown_close"></a>
### leveldown#close()
<code>close()</code> is an instance method on an existing database object. 


--------------------------------------------------------
<a name="leveldown_put"></a>
### leveldown#put()
<code>put()</code> is an instance method on an existing database object. 


--------------------------------------------------------
<a name="leveldown_get"></a>
### leveldown#get()
<code>get()</code> is an instance method on an existing database object. 


--------------------------------------------------------
<a name="leveldown_del"></a>
### leveldown#batch()
<code>batch()</code> is an instance method on an existing database object. 


--------------------------------------------------------
<a name="leveldown_approximateSize"></a>
### leveldown#approximateSize()
<code>approximateSize()</code> is an instance method on an existing database object. 


--------------------------------------------------------
<a name="iterator_next"></a>
### iterator#next()
<code>next()</code> is an instance method on an existing iterator object. 


--------------------------------------------------------
<a name="iterator_end"></a>
### iterator#end()
<code>end()</code> is an instance method on an existing iterator object. 


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
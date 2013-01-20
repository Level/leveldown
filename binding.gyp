{
    "targets": [{
        "target_name": "leveldown"
      , "dependencies": [
            "<(module_root_dir)/deps/leveldb/leveldb.gyp:leveldb"
        ]
      , "sources": [
            "src/async.cc"
          , "src/batch.cc"
          , "src/database.cc"
          , "src/database_async.cc"
          , "src/iterator.cc"
          , "src/iterator_async.cc"
          , "src/leveldown.cc"
        ]
    }]
}

{
    "targets": [{
        "target_name"       : "leveldown"
      , "type"              : "shared_library"
      , "product_extension" : "node"
      , "dependencies"      : [
            "<(module_root_dir)/deps/leveldb/leveldb.gyp:leveldb"
        ]
      , "conditions"        : [
            ["OS=='mac'", {
                "libraries": [
                    "-Wl,-force_load,<(module_root_dir)/build/$(BUILDTYPE)/leveldown.a"
                ]
            }]
          , ["OS=='win'", {
                # see https://github.com/TooTallNate/node-ogg/blob/4fa63ba69a43be08b81ab2e9f9a4fe3fdddb2a58/binding.gyp#L23
                # when this needs to be implemented
            }]
        ]
      , "sources"           : [
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

{
    "targets": [{
        "target_name": "leveldown"
    , 'conditions': [
          ['OS == "win"', {
              'defines': [
                  '_HAS_EXCEPTIONS=0'
              ]
            , 'msvs_settings': {
                  'VCCLCompilerTool': {
                      'RuntimeTypeInfo': 'false'
                    , 'EnableFunctionLevelLinking': 'true'
                    , 'ExceptionHandling': '2'
                    , 'DisableSpecificWarnings': [ '4355', '4530' ,'4267', '4244', '4506' ]
                  }
              }
           }]
        ]
      , "dependencies": [
            "<(module_root_dir)/deps/leveldb/leveldb.gyp:leveldb"
        ]
      , "sources": [
            "src/async.cc"
          , "src/batch.cc"
          , "src/batch_async.cc"
          , "src/database.cc"
          , "src/database_async.cc"
          , "src/iterator.cc"
          , "src/iterator_async.cc"
          , "src/leveldown.cc"
          , "src/leveldown_async.cc"
        ]
    }]
}

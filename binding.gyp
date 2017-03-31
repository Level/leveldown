{
    "targets": [{
      "target_name": "leveldown"
    , "conditions": [
          ["OS == 'win'", {
              "defines": [
                  "_HAS_EXCEPTIONS=0"
              ]
            , "msvs_settings": {
                  "VCCLCompilerTool": {
                      "RuntimeTypeInfo": "false"
                    , "EnableFunctionLevelLinking": "true"
                    , "ExceptionHandling": "2"
                    , "DisableSpecificWarnings": [ "4355", "4530" ,"4267", "4244", "4506" ]
                  }
              }
          }, { # OS != 'win'
              'cflags!': [ '-fno-rtti' ]
            , 'cflags_cc!': [ '-fno-rtti' ]
            , 'cflags_cc+': [ '-frtti' ]
          }]
        , ["OS == 'mac'", {
            'xcode_settings': {
                    'WARNING_CFLAGS': [
                        '-Wno-sign-compare'
                      , '-Wno-unused-variable'
                      , '-Wno-unused-function'
                      , '-Wno-ignored-qualifiers'
                    ]
                    , 'OTHER_CPLUSPLUSFLAGS': [
                        '-mmacosx-version-min=10.8'
                      , '-std=c++11'
                      , '-stdlib=libc++'
                    ]
, 'OTHER_LDFLAGS': ['-stdlib=libc++']
                    , 'GCC_ENABLE_CPP_RTTI': 'YES'
                    , 'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                    , 'MACOSX_DEPLOYMENT_TARGET': '10.8'
            }
          }]
        , ['OS == "linux"', {
              'cflags': []
            , 'cflags!': [ '-fno-tree-vrp' ]
          }]
        ]
      , "dependencies": [
            "<(module_root_dir)/deps/leveldb/leveldb.gyp:leveldb"
        ]
      , "include_dirs"  : [
            "<!(node -e \"require('nan')\")"
        ]
      , "sources": [
            "src/batch.cc"
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

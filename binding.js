const bindings = require('bindings')
    , opts     = {
        bindings : 'leveldown'
      , try      : [
            [ 'module_root', 'build-pre-gyp', 'bindings' ]
          , [ 'module_root', 'build', 'Debug', 'bindings' ]
          , [ 'module_root', 'build', 'Release', 'bindings' ]
        ]
      }

module.exports = bindings(opts).leveldown

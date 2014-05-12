var binding = require('bindings')('leveldown.node').leveldown
  , wrapper = function (options) {
      var instance = binding(options)

      instance.iterator = require('./lib/iterator')(instance)

      return instance
    }

wrapper.destroy = binding.destroy
wrapper.repair = binding.repair

module.exports = wrapper
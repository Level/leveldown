var binding = require('bindings')('leveldown.node')
var leveldown = binding.leveldown
leveldown.CBatch = binding.CBatch
module.exports = leveldown

#include <napi-macros.h>
#include <node_api.h>

// TODO add to napi-macros.h
#define NAPI_RETURN_UNDEFINED() \
  return 0;

NAPI_METHOD(leveldown) {
  NAPI_RETURN_UNDEFINED();
}

NAPI_INIT() {
  NAPI_EXPORT_FUNCTION(leveldown);
}

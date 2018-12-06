#include <napi-macros.h>
#include <node_api.h>

#define NAPI_DB_CONTEXT() \
  DbContext* dbContext = NULL; \
  NAPI_STATUS_THROWS(napi_get_value_external(env, argv[0], (void**)&dbContext));

// TODO move to napi-macros.h
#define NAPI_RETURN_UNDEFINED() \
  return 0;

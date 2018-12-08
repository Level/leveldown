#include <napi-macros.h>
#include <node_api.h>

#define NAPI_DB_CONTEXT() \
  Database* database = NULL; \
  NAPI_STATUS_THROWS(napi_get_value_external(env, argv[0], (void**)&database));

#define NAPI_ITERATOR_CONTEXT() \
  Iterator* iterator = NULL; \
  NAPI_STATUS_THROWS(napi_get_value_external(env, argv[0], (void**)&iterator));

// TODO move to napi-macros.h
#define NAPI_RETURN_UNDEFINED() \
  return 0;

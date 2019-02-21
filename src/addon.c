// addon.c
#include "addon.h"

napi_value create_addon(napi_env env) {
  napi_value result;
  NAPI_ASSERT(napi_create_object(env, &result));
  return result;
}
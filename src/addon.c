#include "addon.h"

napi_value Server(napi_env env, napi_callback_info info)
{
  napi_value ret;
  return ret;
}

napi_value listen(napi_env env, napi_callback_info info)
{
  napi_value ret;
  return ret;
}

napi_value create_addon(napi_env env, napi_value exports)
{
  napi_value nsr;
  napi_property_descriptor methods[] = {
      {"listen", NULL, listen},
  };

  NAPI_ASSERT(napi_define_class(env, "Server", NAPI_AUTO_LENGTH, Server, NULL, 1, methods, &nsr));

  NAPI_ASSERT(napi_set_named_property(env, exports, "Server", nsr));

  return exports;
}
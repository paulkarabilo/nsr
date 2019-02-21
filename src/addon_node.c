#include <node_api.h>

static napi_value Init(napi_env env, napi_value exports)
{
  return create_addon(env, exports);
}

NAPI_MODULE(napi_experiment, Init)
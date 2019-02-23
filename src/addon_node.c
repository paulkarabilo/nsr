#include <node_api.h>
#include "addon.h"
EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
  return create_addon(env, exports);
}
EXTERN_C_END
NAPI_MODULE(napi_experiment, Init)
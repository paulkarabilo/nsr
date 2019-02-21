#ifndef _ADDON_H_
#define _ADDON_H_
#include <assert.h>
#include <js_native_api.h>
#define NAPI_ASSERT(c) (assert((c) == napi_ok))
napi_value create_addon(napi_env env, napi_value exports);
#endif // _ADDON_H_
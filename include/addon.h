#ifndef _ADDON_H_
#define _ADDON_H_
#include <node_version.h>
#include <assert.h>
#if NODE_MAJOR_VERSION == 11
#include <js_native_api.h>
#else
#include <node_api.h>
#endif

#define NAPI_ASSERT(c) (assert((c) == napi_ok))
#define DECLARE_NAPI_PROPERTY(name, func)                                \
  { (name), NULL, (func), NULL, NULL, NULL, napi_default, NULL }

#define NAPI_METHOD_HEADER(env, info, size) \
  size_t argc = size; \
  napi_value args[size]; \
  napi_value _this; \
  nsr_srv_t* srv; \
  NAPI_ASSERT(napi_get_cb_info((env), (info), &argc, args, &_this, NULL));

#define NAPI_METHOD_HEADER_VA_START(env, info) \
  size_t argc; \
  napi_value _this; \
  nsr_srv_t* srv; \
  NAPI_ASSERT(napi_get_cb_info(env, info, &argc, NULL, NULL, NULL)); \
  napi_value* args = calloc(argc, sizeof(napi_value)); \
  NAPI_ASSERT(napi_get_cb_info(env, info, &argc, args, &_this, NULL));

#define NAPI_METHOD_HEADER_VA_END free(args);

napi_value create_addon(napi_env env, napi_value exports);

#endif // _ADDON_H_
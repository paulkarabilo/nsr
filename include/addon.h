#ifndef _ADDON_H_
#define _ADDON_H_
#include <node_version.h>
#include <assert.h>
#if NODE_MAJOR_VERSION == 11
#include <js_native_api.h>
#else
#include <node_api.h>
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define NAPI_VOID

#define THROW_LAST_ERROR(env)                                            \
  const napi_extended_error_info *err;                                   \
  napi_get_last_error_info((env), &err);                                 \
  bool is_pending;                                                       \
  napi_is_exception_pending((env), &is_pending);                         \
  /* If an exception is already pending, don't rethrow it */             \
  if (!is_pending) {                                                     \
    const char* msg = err->error_message != NULL ?                       \
      err->error_message :                                               \
      "empty error message";                                             \
    napi_throw_error((env), NULL, msg);                                  \
  }                                                                      \

#define NAPI_ASSERT_BASE(env, a, m, r)                                   \
  if (!(a)) {                                                            \
    napi_throw_error((env), NULL, "Exception (" #a "): " m);             \
    return r;                                                            \
  }                                                                      \

#define NAPI_ASSERT(env, assertion, message)                             \
  NAPI_ASSERT_BASE(env, assertion, message, NULL)

#define NAPI_ASSERT_NORET(env, assertion, message)                       \
  NAPI_ASSERT_BASE(env, assertion, message, NAPI_VOID)

#define NAPI_CALL_BASE(env, c, r)                                        \
  if ((c) != napi_ok) {                                                  \
    THROW_LAST_ERROR((env));                                             \
    return r;                                                            \
  }                                                                      \

#define NAPI_CALL(env, c)                                                \
  NAPI_CALL_BASE(env, c, NULL)

#define NAPI_CALL_NORET(env, c)                                          \
  NAPI_CALL_BASE(env, c, NAPI_VOID)

#define NAPI_PROPERTY(name, f)                                           \
  { (name), NULL, (f), NULL, NULL, NULL, napi_default, NULL }

#define NAPI_GETTER(name, f)                                             \
  { (name), NULL, NULL, (f), NULL, NULL, napi_default, NULL }

#define NAPI_METHOD_HEADER(env, info, size)                                     \
  size_t argc = (size);                                                           \
  napi_value args[(size)];                                                        \
  napi_value _this;                                                             \
  NAPI_CALL((env),napi_get_cb_info((env), (info), &argc, args, &_this, NULL));

#define NAPI_METHOD_HEADER_WITH_ARG_ASSERT(env, info, size, m)                     \
  NAPI_METHOD_HEADER(env, info, size)                                                          \
  if (argc != size) {\
    napi_throw_error((env), NULL, "method '" m "' expected " STR(size) " arguments"); \
    return NULL; \
  } \


#define NAPI_METHOD_HEADER_VA_START(env, info)                                  \
  size_t argc;                                                                  \
  napi_value _this;                                                             \
  NAPI_CALL((env), napi_get_cb_info(env, info, &argc, NULL, NULL, NULL));       \
  napi_value* args = calloc(argc, sizeof(napi_value));                          \
  NAPI_CALL((env), napi_get_cb_info(env, info, &argc, args, &_this, NULL));

#define NAPI_METHOD_HEADER_WITH_MIN_ARG_ASSERT(env, info, size, m)                     \
  NAPI_METHOD_HEADER_VA_START(env, info)                                                   \
  if (argc < size) {                                                                   \
    napi_throw_error((env), NULL, "method '" m "' expected at least " STR(size) " arguments"); \
    return NULL; \
  } \

#define NAPI_METHOD_EXPECT_ARG_TYPE(env, n, type, m) {                                                    \
  napi_valuetype t;                                                                                       \
  NAPI_CALL((env), napi_typeof(env, args[n], &t));                                                        \
  if (t != (napi_##type)) {                                                                               \
    napi_throw_error((env), NULL, "method '" m "' expected argument " STR(n) " to be of type " STR(type));\
    return NULL;                                                                                          \
  }                                                                                                       \
}



#define NAPI_METHOD_HEADER_VA_END free(args);


napi_value create_addon(napi_env env, napi_value exports);

#endif // _ADDON_H_
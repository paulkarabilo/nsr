#include "addon.h"

static napi_ref nsr_ctor;

typedef struct server_t {
  int port;
} server_t;

void destroy(napi_env env, void* object, void* hint) {
  free(object);
}


napi_value Server(napi_env env, napi_callback_info info)
{
  napi_value _this;
  server_t* srv = malloc(sizeof(server_t));
  printf("CREATING\n");
  NAPI_ASSERT(napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));
  NAPI_ASSERT(napi_wrap(env, _this, srv, destroy, NULL, NULL));
  return _this;
}

static napi_value listen(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  NAPI_ASSERT(napi_get_cb_info(env, info, &argc, args, NULL, NULL));

  NAPI_ASSERT(argc == 1);
  printf("CALLING\n");
  return args[0];
}

napi_value create_addon(napi_env env, napi_value exports)
{
  napi_property_descriptor methods[] = {
    DECLARE_NAPI_PROPERTY("listen", listen),
  };
  napi_value nsr;
  NAPI_ASSERT(napi_define_class(env, "Server", -1, Server, NULL, 1, methods, &nsr));
  NAPI_ASSERT(napi_create_reference(env, nsr, 1, &nsr_ctor));
  NAPI_ASSERT(napi_set_named_property(env, exports, "Server", nsr));

  return exports;
}
#include "addon.h"
#include "srv.h"

static napi_ref nsr_ctor;

void destroy(napi_env env, void* object, void* hint) {
  nsr_srv_free((nsr_srv_t*)object);
}

napi_value Server(napi_env env, napi_callback_info info) {
  napi_value _this;
  struct uv_loop_s* loop;
  NAPI_ASSERT(napi_get_uv_event_loop(env, &loop));
  nsr_srv_t* srv = nsr_srv_init(loop);
  NAPI_ASSERT(napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));
  NAPI_ASSERT(napi_wrap(env, _this, srv, destroy, NULL, NULL));
  return _this;
}

static napi_value listen(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  napi_value _this;
  nsr_srv_t* srv;
  NAPI_ASSERT(napi_get_cb_info(env, info, &argc, args, &_this, NULL));
  NAPI_ASSERT(napi_unwrap(env, _this, (void*)(&srv)));
  nsr_callback_trigger(env, srv->callbacks, "connect", 0, NULL);
  NAPI_ASSERT(argc == 1);
  return NULL;
}

static napi_value on(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[2];
  napi_value _this;
  nsr_srv_t* srv;
  NAPI_ASSERT(napi_get_cb_info(env, info, &argc, args, &_this, NULL));
  NAPI_ASSERT(napi_unwrap(env, _this, (void*)(&srv)));
  size_t length;
  NAPI_ASSERT(napi_get_value_string_utf8(env, args[0], NULL, 0, &length));
  char* key = calloc(length + 1, sizeof(char));
  NAPI_ASSERT(napi_get_value_string_utf8(env, args[0], key, length + 1, NULL));
  napi_ref ref;
  NAPI_ASSERT(napi_create_reference(env, args[1], 1, &ref));
  nsr_callback_map_add(srv->callbacks, key, ref);
  free(key);
  return NULL;
}

static napi_value emit(napi_env env, napi_callback_info info) {
  size_t argc;
  napi_value _this;
  nsr_srv_t* srv;
  NAPI_ASSERT(napi_get_cb_info(env, info, &argc, NULL, NULL, NULL));
  napi_value* args = calloc(argc, sizeof(napi_value));
  NAPI_ASSERT(napi_get_cb_info(env, info, &argc, args, &_this, NULL));
  NAPI_ASSERT(napi_unwrap(env, _this, (void*)(&srv)));
  size_t length;
  NAPI_ASSERT(napi_get_value_string_utf8(env, args[0], NULL, 0, &length));
  char* key = calloc(length + 1, sizeof(char));
  NAPI_ASSERT(napi_get_value_string_utf8(env, args[0], key, length + 1, NULL));
  nsr_callback_trigger(env, srv->callbacks, key, argc - 1, args + 1);
  free(args);
  free(key);
  return NULL;
}


napi_value create_addon(napi_env env, napi_value exports) {
  napi_property_descriptor methods[] = {
    DECLARE_NAPI_PROPERTY("listen", listen),
    DECLARE_NAPI_PROPERTY("on", on),
    DECLARE_NAPI_PROPERTY("emit", emit),
  };
  napi_value nsr;
  NAPI_ASSERT(napi_define_class(env, "Server", -1, Server, NULL, 3, methods, &nsr));
  NAPI_ASSERT(napi_create_reference(env, nsr, 1, &nsr_ctor));
  NAPI_ASSERT(napi_set_named_property(env, exports, "Server", nsr));

  return exports;
}

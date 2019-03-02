#include "addon.h"
#include "srv.h"

static napi_ref nsr_ctor;

void destroy(napi_env env, void* object, void* hint) {
  nsr_srv_free(env, (nsr_srv_t*)object);
}

char* value_to_str(napi_env env, napi_value val) {
  size_t length;
  NAPI_CALL(env, napi_get_value_string_utf8(env, val, NULL, 0, &length));
  char* s = calloc(length + 1, sizeof(char));
  NAPI_CALL(env, napi_get_value_string_utf8(env, val, s, length + 1, NULL));
  return s;
}

napi_value Server(napi_env env, napi_callback_info info) {
  napi_value _this;
  struct uv_loop_s* loop;
  NAPI_CALL(env, napi_get_uv_event_loop(env, &loop));
  nsr_srv_t* srv = nsr_srv_init(loop);
  NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));
  NAPI_CALL(env, napi_wrap(env, _this, srv, destroy, NULL, NULL));
  return _this;
}

static napi_value do_listen(napi_env env, napi_callback_info info) {
  NAPI_METHOD_HEADER(env, info, 1);
  nsr_srv_t* srv;
  NAPI_CALL(env, napi_unwrap(env, _this, (void*)(&srv)));
  nsr_srv_start(srv, "0.0.0.0", 8000);
  nsr_callback_trigger(env, srv->callbacks, "connect", 0, NULL);
  NAPI_CALL(env, argc == 1);
  return NULL;
}

static napi_value on(napi_env env, napi_callback_info info) {
  NAPI_METHOD_HEADER_WITH_ARG_ASSERT(env, info, 2, "on");
  NAPI_METHOD_EXPECT_ARG_TYPE(env, 0, string, "on");
  NAPI_METHOD_EXPECT_ARG_TYPE(env, 1, function, "on");

  nsr_srv_t* srv;
  NAPI_CALL(env, napi_unwrap(env, _this, (void*)(&srv)));

  char* key = value_to_str(env, args[0]);

  napi_ref ref;
  NAPI_CALL(env, napi_create_reference(env, args[1], 1, &ref));
  nsr_callback_map_add(srv->callbacks, key, ref);

  free(key);
  return NULL;
}

static napi_value emit(napi_env env, napi_callback_info info) {
  NAPI_METHOD_HEADER_WITH_MIN_ARG_ASSERT(env, info, 1, "emit");
  NAPI_METHOD_EXPECT_ARG_TYPE(env, 0, string, "emit");

  nsr_srv_t* srv;
  NAPI_CALL(env, napi_unwrap(env, _this, (void*)(&srv)));

  char* key = value_to_str(env, args[0]);

  nsr_callback_trigger(env, srv->callbacks, key, argc - 1, args + 1);
  free(key);

  NAPI_METHOD_HEADER_VA_END;
  return NULL;
}


napi_value create_addon(napi_env env, napi_value exports) {
  napi_property_descriptor methods[] = {
    NAPI_PROPERTY("listen", do_listen),
    NAPI_PROPERTY("on", on),
    NAPI_PROPERTY("emit", emit),
  };
  napi_value nsr;
  NAPI_CALL(env, napi_define_class(env, "Server", -1, Server, NULL, 3, methods, &nsr));
  NAPI_CALL(env, napi_create_reference(env, nsr, 1, &nsr_ctor));
  NAPI_CALL(env, napi_set_named_property(env, exports, "Server", nsr));

  return exports;
}

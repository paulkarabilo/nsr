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

bool value_of_type(napi_env env, napi_value value, napi_valuetype type) {
  napi_valuetype t;
  NAPI_CALL((env), napi_typeof(env, value, &t));
  return (t == type);
}

void bind_host_port_cb_from_args(napi_env env, int argc, napi_value* args, nsr_srv_t* srv) {
  napi_value _host = NULL;
  napi_value _port = NULL;

  char* host = "0.0.0.0";
  int64_t port = 8080;

  if (argc > 0) {
    bool free_host = false;
    bool host_or_port_bound = false;

    short callback_index = -1;

    if(value_of_type(env, args[0], napi_function)) {
      callback_index = 0;
    }

    if(argc > 1 && value_of_type(env, args[1], napi_function)) {
      callback_index = 1;
    }

    if(callback_index >= 0) {
      napi_ref ref;
      NAPI_CALL_NORET(env, napi_create_reference(env, args[callback_index], 1, &ref));
      nsr_callback_map_add(srv->callbacks, "request", ref);
      host_or_port_bound = true;
    }

    if ((callback_index == 1 || callback_index == -1) && value_of_type(env, args[0], napi_object)) {
      if (napi_get_named_property(env, args[0], "host", &_host) == napi_ok) {
        if (value_of_type(env, _host, napi_string)) {
          host = value_to_str(env, _host);
          free_host = true;
        }
      }
      if (napi_get_named_property(env, args[0], "port", &_port) == napi_ok) {
        if (value_of_type(env, _port, napi_number)) {
          NAPI_CALL_NORET(env, napi_get_value_int64(env, _port, &port))
        }
      }
    }

    if (host_or_port_bound) {
      nsr_srv_start(srv, host, (int)port);
    }

    if (free_host) free(host);
  }
}

napi_value Server(napi_env env, napi_callback_info info) {
  NAPI_METHOD_HEADER_VA_START(env, info);

  struct uv_loop_s* loop;
  NAPI_CALL(env, napi_get_uv_event_loop(env, &loop));

  nsr_srv_t* srv = nsr_srv_init(loop, env);
  NAPI_CALL(env, napi_wrap(env, _this, srv, destroy, NULL, NULL));

  bind_host_port_cb_from_args(env, argc, args, srv);

  NAPI_METHOD_HEADER_VA_END;

  return _this;
}

static napi_value do_listen(napi_env env, napi_callback_info info) {
  NAPI_METHOD_HEADER_VA_START(env, info);
  nsr_srv_t* srv;
  NAPI_CALL(env, napi_unwrap(env, _this, (void*)(&srv)));
  bind_host_port_cb_from_args(env, argc, args, srv);

  NAPI_METHOD_HEADER_VA_END;
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

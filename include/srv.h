#include <node_api.h>
#include <stdlib.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include "callbacks.h"

typedef struct uv_loop_s uv_loop_t;

typedef struct nsr_srv_s {
  int port;
  char* host;
  uv_loop_t* loop;
  nsr_callback_map_t* callbacks;
} nsr_srv_t;

nsr_srv_t* nsr_srv_init(uv_loop_t* loop);
void nsr_srv_start(nsr_srv_t* src, char* host, int port);
void nsr_srv_free(napi_env env, nsr_srv_t* srv);

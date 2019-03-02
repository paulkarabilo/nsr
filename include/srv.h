#include <node_api.h>
#include <stdlib.h>
#include <node_version.h>
#include <uv.h>
#include <node_api.h>
#if NODE_MAJOR_VERSION == 11
#include <js_native_api.h>
#include <js_native_api_types.h>
#endif
#include "callbacks.h"


typedef struct nsr_srv_s {
  int port;
  char* host;
  uv_loop_t* loop;
  nsr_callback_map_t* callbacks;
  napi_env env;
} nsr_srv_t;

#define UV_CALL_NORET(c, err) if ((rc = (c)) < 0) { \
  printf(err, uv_strerror(rc)); \
}


#define UV_CALL(c, err) if ((rc = (c)) < 0) { \
  printf(err, uv_strerror(rc)); \
  return rc; \
}

nsr_srv_t* nsr_srv_init(uv_loop_t* loop, napi_env env);
int nsr_srv_start(nsr_srv_t* src, char* host, int port);
void nsr_srv_free(napi_env env, nsr_srv_t* srv);

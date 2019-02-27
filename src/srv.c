#include "srv.h"

nsr_srv_t* nsr_srv_init(uv_loop_t* loop) {
  nsr_srv_t* srv = malloc(sizeof(nsr_srv_t));
  srv->callbacks = nsr_callback_map_init();
  srv->loop = loop;
  return srv;
}

void nsr_srv_free(napi_env env, nsr_srv_t* srv) {
  nsr_callback_map_free(env, srv->callbacks);
  free(srv);
}

void on_connect(uv_stream_t* srv_stream, int status) {
  if (status < 0) {
    fprintf(stderr, "Peer connection error: %s\n", uv_strerror(status));
    return;
  }
  printf("CLIENT CONNECTED\n");
}

void nsr_srv_start(nsr_srv_t* srv, char* host, int port) {
  uv_tcp_t srv_stream;
  int rc;
  if ((rc = uv_tcp_init(srv->loop, &srv_stream)) < 0) {
    printf("init err %s\n", uv_strerror(rc));
    return;
  }

  struct sockaddr_in addr;

  if ((rc = uv_ip4_addr(host, port, &addr)) < 0) {
    printf("ip4addr err %s\n", uv_strerror(rc));
    return;
  }

  if ((rc = uv_tcp_bind(&srv_stream, (const struct sockaddr*)&addr, 0)) < 0) {
    printf("bind err %s\n", uv_strerror(rc));
    return;
  }

  if ((rc = uv_listen((uv_stream_t*)&srv_stream, 64, on_connect)) < 0) {
    printf("listen err %s\n", uv_strerror(rc));
    return;
  }

  uv_run(srv->loop, UV_RUN_DEFAULT);

  printf("LISTEN!\n");
}

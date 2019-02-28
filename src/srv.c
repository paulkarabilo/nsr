#include "srv.h"

#include <assert.h>

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

void on_alloc_buffer(uv_handle_t* handle, size_t suggested_size,
                     uv_buf_t* buf) {
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}

void on_client_closed(uv_handle_t* handle) {
  uv_tcp_t* client = (uv_tcp_t*)handle;
  if (client->data) {
    free(client->data);
  }
  free(client);
}

void on_wrote_buf(uv_write_t* req, int status) {
  int rc;
  UV_CALL_NORET(status, "Write error: %s\n");
  free(req);
}

void on_peer_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
  if (nread < 0) {
    if (nread != UV_EOF) {
        fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) client, NULL);
    }
  } else if (nread > 0) {
      uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
      uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
      uv_write(req, client, &wrbuf, 1, on_wrote_buf);
  }

  if (buf->base) {
      free(buf->base);
  }
}

void on_connect(uv_stream_t* srv_stream, int status) {
  int rc;
  UV_CALL_NORET(status, "Peer connection error: %s\n");

  uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(*client));
  UV_CALL_NORET(uv_tcp_init(uv_default_loop(), client), "uv_tcp_init %s");
  client->data = NULL;

  if (uv_accept(srv_stream, (uv_stream_t*)client) == 0) {
    UV_CALL_NORET(uv_read_start((uv_stream_t*)client, on_alloc_buffer, on_peer_read), "uv_read_start: %s");
  } else {
    uv_close((uv_handle_t*)client, on_client_closed);
  }
}

int nsr_srv_start(nsr_srv_t* srv, char* host, int port) {
  uv_tcp_t srv_stream;
  int rc;
  UV_CALL(uv_tcp_init(srv->loop, &srv_stream), "init %s");

  struct sockaddr_in addr;
  UV_CALL(uv_ip4_addr(host, port, &addr), "ip4addr %s");
  UV_CALL(uv_tcp_bind(&srv_stream, (const struct sockaddr*)&addr, 0), "bind %s");
  UV_CALL(uv_listen((uv_stream_t*)&srv_stream, 64, on_connect), "listen %s");

  uv_run(srv->loop, UV_RUN_DEFAULT);
  return 0;
}

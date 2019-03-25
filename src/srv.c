#include "srv.h"
#include "addon.h"
#include <assert.h>

nsr_srv_t* nsr_srv_init(uv_loop_t* loop, napi_env env) {
  nsr_srv_t* srv = malloc(sizeof(nsr_srv_t));
  srv->env = env;
  srv->callbacks = nsr_callback_map_init();
  srv->loop = loop;
  return srv;
}

void nsr_srv_free(napi_env env, nsr_srv_t* srv) {
  nsr_callback_map_free(env, srv->callbacks);
  free(srv);
}

void alloc_buf(uv_handle_t* handle, size_t s, uv_buf_t* buf) {
  buf->base = (char*)malloc(s);
  buf->len = s;
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

void peer_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
  nsr_srv_t* srv = (nsr_srv_t*)client->data;

  if (nread < 0) {
    if (nread != UV_EOF) {
        fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) client, NULL);
    }
  } else if (nread > 0) {
      napi_value args[1];
      napi_handle_scope scope;
      NAPI_CALL_NORET(srv->env, napi_open_handle_scope(srv->env, &scope));
      NAPI_CALL_NORET(srv->env, napi_create_string_utf8(srv->env, buf->base, nread, args));
      nsr_callback_trigger(srv->env, srv->callbacks, "request", 1, args);
      NAPI_CALL_NORET(srv->env, napi_close_handle_scope(srv->env, scope));
  }

  if (buf->base) {
      free(buf->base);
  }
}

void on_connect(uv_stream_t* srv_stream, int status) {
  int rc;
  UV_CALL_NORET(status, "Peer connection error: %s\n");
  nsr_srv_t* srv = (nsr_srv_t*)srv_stream->data;

  uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(*client));

  UV_CALL_NORET(uv_tcp_init(srv->loop, client), "uv_tcp_init %s");
  client->data = srv_stream->data;

  if (uv_accept(srv_stream, (uv_stream_t*)client) == 0) {
    UV_CALL_NORET(uv_read_start((uv_stream_t*)client, alloc_buf, peer_read), "uv_read_start: %s");
  } else {
    uv_close((uv_handle_t*)client, on_client_closed);
  }
}

int nsr_srv_start(nsr_srv_t* srv, char* host, int port) {
  uv_tcp_t* tcp = malloc(sizeof(uv_tcp_t));
  int rc;
  UV_CALL(uv_tcp_init(srv->loop, tcp), "init %s");
  tcp->data = (void*)srv;
  srv->handle = tcp;

  struct sockaddr_in addr;
  UV_CALL(uv_ip4_addr(host, port, &addr), "ip4addr %s");
  UV_CALL(uv_tcp_bind(tcp, (const struct sockaddr*)(&addr), 0), "bind %s");
  UV_CALL(uv_listen((uv_stream_t*)(tcp), 64, on_connect), "listen %s");
  uv_unref((uv_handle_t*)tcp);
  return 0;
}

void close_cb(uv_handle_t* handle) {
  free(handle);
}

void nsr_srv_close(nsr_srv_t* srv) {
  uv_close((uv_handle_t*)srv->handle, close_cb);
}

#include "srv.h"

nsr_srv_t* nsr_srv_init(uv_loop_t* loop) {
  nsr_srv_t* srv = malloc(sizeof(nsr_srv_t));
  srv->callbacks = nsr_callback_map_init();
  srv->loop = loop;
  return srv;
}

void nsr_srv_free(nsr_srv_t* srv) {
  nsr_callback_map_free(srv->callbacks);
  free(srv);
}

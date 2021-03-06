#include <node_api.h>
#include <stdlib.h>
#include <string.h>
#define CALLBACK_BUCKET_COUNT 128

typedef struct nsr_callback_item_s {
  char* key;
  int count;
  int size;
  napi_ref* functions;
  struct nsr_callback_item_s* next;
} nsr_callback_item_t;

typedef struct nsr_callback_map_s {
  nsr_callback_item_t* items[CALLBACK_BUCKET_COUNT];
} nsr_callback_map_t;

nsr_callback_map_t* nsr_callback_map_init();
nsr_callback_item_t* nsr_callback_item_init(char* key, napi_ref fn);
void nsr_callback_map_free(napi_env env, nsr_callback_map_t* map);
void nsr_callback_item_free(napi_env env, nsr_callback_item_t* item);
void nsr_callback_map_add(nsr_callback_map_t* map, char* key, napi_ref fn);
void nsr_callback_trigger(napi_env env, nsr_callback_map_t* map, char* event, int argc, napi_value* argv);

#include <node_api.h>

typedef struct nsr_callback_item_s {
  char* key;
  int count;
  int size;
  napi_value** functions;
} nsr_callback_item_t;

typedef struct nsr_callback_map_s {
  int size;
  int count;
  nsr_callback_item_t** items;
} nsr_callback_map_t;

nsr_callback_map_t* nsr_callback_map_init();
nsr_callback_item_t* nsr_callback_item_init(char* key);
void nsr_callback_map_free(nsr_callback_map_t* map);
void nsr_callback_item_free(nsr_callback_item_t* item);

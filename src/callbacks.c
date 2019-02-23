#include "callbacks.h"

nsr_callback_map_t* nsr_callback_map_init() {
  nsr_callback_map_t* map = malloc(sizeof(nsr_callback_map_t));
  map->size = 128;
  map->count = 0;
  map->items = calloc(map->size, sizeof(nsr_callback_item_t*));
  return map;
}

nsr_callback_item_t* nsr_callback_item_init(char* key) {
  nsr_callback_item_t* item = malloc(sizeof(nsr_callback_item_t));
  item->key = strdup(key);
  item->count = 0;
  item->size = 128;
  item->functions = calloc(item->size, sizeof(nsr_callback_item_t*));
  return item;
}

void nsr_callback_item_free(nsr_callback_item_t* item) {
  free(item->key);
  for (int i = 0; i < item->count; i++) {
    if (item->functions[i] != NULL) {
      free(item->functions[i]);
    }
  }
  free(item->functions);
  free(item);
}

void nsr_callback_map_free(nsr_callback_map_t* map) {
  for (int i = 0; i < map->count; i++) {
    nsr_callback_item_free(map->items[i]);
  }
  free(map->items);
  free(map);
}

#include "callbacks.h"
#include "addon.h"

static int hash(char* s) {
  unsigned long hash = 5381;
  int c;
  while (c = *s++) {
    hash = ((hash << 5) + hash) + c;
  }
  return (int)(hash % CALLBACK_BUCKET_COUNT);
}

nsr_callback_map_t* nsr_callback_map_init() {
  nsr_callback_map_t* map = malloc(sizeof(nsr_callback_map_t));
  for (int i = 0; i < CALLBACK_BUCKET_COUNT; i++) {
    map->items[i] = NULL;
  }
  return map;
}

nsr_callback_item_t* nsr_callback_item_init(char* key, napi_ref fn) {
  nsr_callback_item_t* item = malloc(sizeof(nsr_callback_item_t));
  item->key = strdup(key);
  item->size = 128;
  item->functions = calloc(item->size, sizeof(napi_ref));
  item->functions[0] = fn;
  item->count = 1;
  item->next = NULL;
  return item;
}

void nsr_callback_item_grow(nsr_callback_item_t* item) {
  item->size *= 2;
  item->functions = realloc(item->functions, item->size * sizeof(napi_ref));
}

void nsr_callback_item_add(nsr_callback_item_t* item, napi_ref fn) {
  if (item->count >= item->size) {
    nsr_callback_item_grow(item);
  }
  item->functions[item->count] = fn;
  item->count++;
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
  for (int i = 0; i < CALLBACK_BUCKET_COUNT; i++) {
    nsr_callback_item_free(map->items[i]);
  }
  free(map->items);
  free(map);
}

void nsr_callback_map_add(nsr_callback_map_t* map, char* key, napi_ref fn) {
  int h = hash(key);
  nsr_callback_item_t* item = map->items[h];
  if (item == NULL) {
    map->items[h] = nsr_callback_item_init(key, fn);
  } else {
    while (item != NULL) {
      if (strcmp(item->key, key) == 0) {
        nsr_callback_item_add(item, fn);
        return;
      } else if (item->next == NULL) {
        item->next = nsr_callback_item_init(key, fn);
      } else {
        item = item->next;
      }
    }
  }
}

void nsr_callback_trigger(napi_env env, nsr_callback_map_t* map, char* event, int argc, napi_value* argv) {
  int h = hash(event);
  nsr_callback_item_t* item = map->items[h];
  napi_value global;
  NAPI_ASSERT(napi_get_global(env, &global));
  while (item != NULL) {
    if (strcmp(item->key, event) == 0) {
      for (int i = 0; i < item->count; i++) {
        napi_value fn;
        NAPI_ASSERT(napi_get_reference_value(env, item->functions[i], &fn));
        NAPI_ASSERT(napi_call_function(env, global, fn, argc, argv, NULL));
      }
    }
    item = item->next;
  }
}

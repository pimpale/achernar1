#include <stdlib.h>
#include <string.h>

#include "map.h"

void map_mapping_init(struct map_mapping* mapping, void* key, void* value,
                      size_t keylen, size_t valuelen) {
  mapping->keylen = keylen;
  mapping->valuelen = valuelen;
  mapping->key = malloc(keylen);
  mapping->value = malloc(valuelen);
  memcpy(mapping->key, key, keylen);
  memcpy(mapping->value, value, valuelen);
  mapping->next = NULL;
}

void map_mapping_free(struct map_mapping* mapping) {
  free(mapping->key);
  free(mapping->value);
}

void map_init(Map* map) { map->first = NULL; }

void map_free(Map* map) {
  struct map_mapping* mapping = map->first;
  // Free each mapping
  while (mapping != NULL) {
    struct map_mapping* next_mapping = mapping->next;
    map_mapping_free(mapping);
    mapping = next_mapping;
  }
}

void map_put(Map* map, void* key, void* value, size_t* keylen,
             size_t* valuelen) {
  // Create new mapping
  struct map_mapping* new_mapping = malloc(sizeof(struct map_mapping));
  map_mapping_init(new_mapping, key, value, *keylen, *valuelen);

  // Append to last part of list
  if (map->first != NULL) {
    struct map_mapping* mapping = map->first;
    while (mapping->next != NULL) {
      mapping = mapping->next;
    }
    mapping->next = new_mapping;
  } else {
    map->first = new_mapping;
  }
}

void map_get(Map* map, void* key, void* value, size_t* keylen,
             size_t* valuelen) {
  struct map_mapping* mapping = map->first;
  while (mapping != NULL) {
    if (!memcmp(mapping->key, key, mapping->keylen)) {
      memcpy(value, mapping->value, mapping->valuelen);
      *valuelen = mapping->valuelen;
      return;
    }
    mapping = mapping->next;
  }
  *valuelen = 0;
}

void map_del(Map* map, void* key, size_t keylen) {
  struct map_mapping* mapping = map->first;
  struct map_mapping* previous = NULL;
  while (mapping != NULL) {
    if (!memcmp(mapping->key, key, mapping->keylen)) {
      if(mapping->next != NULL) {
        if(previous != NULL) {
          previous->next = mapping->next;
        }
      } else {
        if(previous != NULL) {
          previous->next = NULL;
        }
      }
      //if it is the first element in the list
      if(mapping == map->first) {
        map->first = NULL;
      }
      map_mapping_free(mapping);
      return;
    }
    previous = mapping;
    mapping = mapping->next;
  }
}

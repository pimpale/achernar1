#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <string.h>

// TODO make map not a linkedlist

struct Mapping {
  void *key;
  size_t keylen;
  void *value;
  size_t valuelen;

  struct Mapping *next;
};

typedef struct {
  struct Mapping *first;
} Map;

/* Initialize the map */
void initMap(Map *map);
/* Delete the map */
void freeMap(Map *map);

/* Puts a Mapping in the map */
void putMap(Map *map, void *key, void *value, size_t *keylen, size_t *valuelen);
/* Retrieves the value of the map given the key
 * If the value is found, the */
void getMap(Map *map, void *key, void *value, size_t *keylen, size_t *valuelen);
/* Deletes the Mapping given by key */
void removeMap(Map *map, void *key, size_t keylen);

void Mapping_init(struct Mapping *mapping, void *key, void *value,
                  size_t keylen, size_t valuelen) {
  mapping->keylen = keylen;
  mapping->valuelen = valuelen;
  mapping->key = malloc(keylen);
  mapping->value = malloc(valuelen);
  memcpy(mapping->key, key, keylen);
  memcpy(mapping->value, value, valuelen);
  mapping->next = NULL;
}

void Mapping_free(struct Mapping *mapping) {
  free(mapping->key);
  free(mapping->value);
}

void initMap(Map *map) { map->first = NULL; }

void freeMap(Map *map) {
  struct Mapping *mapping = map->first;
  // Free each mapping
  while (mapping != NULL) {
    struct Mapping *next_mapping = mapping->next;
    Mapping_free(mapping);
    mapping = next_mapping;
  }
}

void putMap(Map *map, void *key, void *value, size_t *keylen,
             size_t *valuelen) {
  // Create new mapping
  struct Mapping *new_mapping = malloc(sizeof(struct Mapping));
  Mapping_init(new_mapping, key, value, *keylen, *valuelen);

  // Append to last part of list
  if (map->first != NULL) {
    struct Mapping *mapping = map->first;
    while (mapping->next != NULL) {
      mapping = mapping->next;
    }
    mapping->next = new_mapping;
  } else {
    map->first = new_mapping;
  }
}

void getMap(Map *map, void *key, void *value, size_t *keylen,
             size_t *valuelen) {
  struct Mapping *mapping = map->first;
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

void delMap(Map *map, void *key, size_t keylen) {
  struct Mapping *mapping = map->first;
  struct Mapping *previous = NULL;
  while (mapping != NULL) {
    if (!memcmp(mapping->key, key, mapping->keylen)) {
      if (mapping->next != NULL) {
        if (previous != NULL) {
          previous->next = mapping->next;
        }
      } else {
        if (previous != NULL) {
          previous->next = NULL;
        }
      }
      // if it is the first element in the list
      if (mapping == map->first) {
        map->first = NULL;
      }
      Mapping_free(mapping);
      return;
    }
    previous = mapping;
    mapping = mapping->next;
  }
}

#endif

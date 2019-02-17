#include <stdlib.h>
#include <string.h>

#include "table.h"

void initMapping(struct Mapping *mapping, void *key, void *value,
                    size_t keylen, size_t valuelen) {
  mapping->keylen = keylen;
  mapping->valuelen = valuelen;
  mapping->key = malloc(keylen);
  mapping->value = malloc(valuelen);
  memcpy(mapping->key, key, keylen);
  memcpy(mapping->value, value, valuelen);
  mapping->next = NULL;
}

void freeMapping(struct Mapping *mapping) {
  free(mapping->key);
  free(mapping->value);
}

void initTable(Table *table) { table->first = NULL; }

void freeTable(Table *table) {
  struct Mapping *mapping = table->first;
  // Free each mapping
  while (mapping != NULL) {
    struct Mapping *next_mapping = mapping->next;
    freeMapping(mapping);
    mapping = next_mapping;
  }
}

void putTable(Table *table, void *key, void *value, size_t keylen,
              size_t valuelen) {
  // Delete old mapping
  delTable(table, key, keylen);

  // Create new mapping
  struct Mapping *new_mapping = malloc(sizeof(struct Mapping));
  initMapping(new_mapping, key, value, keylen, valuelen);

  // Append to last part of list
  if (table->first != NULL) {
    struct Mapping *mapping = table->first;
    while (mapping->next != NULL) {
      mapping = mapping->next;
    }
    mapping->next = new_mapping;
  } else {
    table->first = new_mapping;
  }
}

void delTable(Table *table, void *key, size_t keylen) {
  struct Mapping *mapping = table->first;
  struct Mapping *previous = NULL;
  while (mapping != NULL) {
    if (keylen == mapping->keylen &&
        !memcmp(mapping->key, key, mapping->keylen)) {
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
      if (mapping == table->first) {
        table->first = NULL;
      }
      freeMapping(mapping);
      return;
    }
    previous = mapping;
    mapping = mapping->next;
  }
}

void getTable(Table *table, void *key, void *value, size_t keylen,
              size_t *valuelen) {
  struct Mapping *mapping = table->first;
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

#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "table.h"

void initMapping(Mapping *mapping, void *key, void *value, size_t keylen,
                 size_t valuelen) {
  mapping->keylen = keylen;
  mapping->valuelen = valuelen;
  mapping->key = malloc(keylen);
  mapping->value = malloc(valuelen);
  memcpy(mapping->key, key, keylen);
  memcpy(mapping->value, value, valuelen);
}

void freeMapping(Mapping *mapping) {
  free(mapping->key);
  free(mapping->value);
}

void initTable(Table *table) { 
  initVector(&table->mappings);
}

void freeTable(Table *table) {
  // Free all mappings

  

  // Now free vector
  freeVector(&table->mappings);
}

void putTable(Table *table, void *key, void *value, size_t keylen,
              size_t valuelen) {
  // Delete old mapping
  delTable(table, key, keylen);

  // Create new mapping
  Mapping *new_mapping = malloc(sizeof(Mapping));
  initMapping(new_mapping, key, value, keylen, valuelen);

  //Append 
  
}

void delTable(Table *table, void *key, size_t keylen) {

}

void getTable(Table *table, void *key, void *value, size_t keylen, size_t *valuelen) {

}

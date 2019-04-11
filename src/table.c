#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "vector.h"

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

void initTable(Table *table) { initVector(&table->mappings); }

void freeTable(Table *table) {
  // Free all mappings
  for (size_t i = 0; i < lengthVector(&table->mappings); i++) {
    freeMapping(VEC_GET(&table->mappings, i, Mapping));
  }
  // Now free vector
  freeVector(&table->mappings);
}

void putTable(Table *table, void *key, void *value, size_t keylen,
              size_t valuelen) {
  // Delete old mapping
  delTable(table, key, keylen);
  // Create new mapping
  Mapping *new_mapping = VEC_PUSH(&table->mappings, Mapping);
  // Initialize mapping
  initMapping(new_mapping, key, value, keylen, valuelen);
}

void delTable(Table *table, void *key, size_t keylen) {
  for (size_t i = 0; i < lengthVector(&table->mappings); i++) {
    Mapping *m = VEC_GET(&table->mappings, i, Mapping);
    // If the keys match
    if(m->keylen == keylen && memcmp(m->key, key, keylen)) {
      VEC_REM(&table->mappings, i, Mapping); 
      return;
    }
  }
}

void getTable(Table *table, void *key, void *value, size_t keylen,
              size_t valuelen) {
  for (size_t i = 0; i < lengthVector(&table->mappings); i++) {
    Mapping *m = VEC_GET(&table->mappings, i, Mapping);
    // If the keys match
    if(m->keylen == keylen && memcmp(m->key, key, keylen)) {
      // Copy valuelen values to the given value
      memcpy(value, m->value, valuelen); 
      return;
    }
  }
}

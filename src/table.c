#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "table.h"
#include "vector.h"
#include "hash.h"

#define DEFAULT_LOAD_FACTOR 0.2
#define INITIAL_CAPACITY 256

/* hash function using linear probing */
size_t hashTable(void* data, size_t datalen, size_t bucketCount, uint32_t attempt);

size_t hashTable(void* data, size_t datalen, size_t bucketCount, uint32_t attempt) {
  return ((simpleHash(data, datalen) + attempt) % bucketCount);
}

typedef struct {
  bool existent;
  void *key;
  size_t keylen;
  void *value;
  size_t valuelen;
} Mapping ;

/* Initializes a mapping data structure */
void initMapping(Mapping *mapping, void *key, size_t keylen, void* value,
    size_t valuelen);

/* Frees a mapping data structure */
void freeMapping(Mapping *mapping);

void initMapping(Mapping *mapping, void *key, size_t keylen, void* value,
    size_t valuelen) {
  mapping->existent = true;
  mapping->keylen = keylen;
  mapping->valuelen = valuelen;
  mapping->key = malloc(keylen);
  mapping->value = malloc(valuelen);
  memcpy(mapping->key, key, keylen);
  memcpy(mapping->value, value, valuelen);
}

void freeMapping(Mapping *mapping) {
  if(mapping->existent) {
    mapping->existent = false;
    free(mapping->key);
    free(mapping->value);
  }
}

void initTable(Table *table) { 
  initTableCapacity(table, INITIAL_CAPACITY);
}

void initTableCapacity(Table *table, size_t capacity) {
  table->mappingCount= 0;
  table->mappingCapacity = capacity;
  table->loadFactor = 0.2f;
  // Initialize vector
  initVector(&table->mappings); 
  // Make space in vector for mappings
  pushVector(&table->mappings, capacity * sizeof(Mapping));
}

// Creates new table, inserts old stuff, deletes this table. Expensive, avoid this operation
void resizeTable(Table *table, size_t newCapacity) {
  Table newTable;
  initTableCapacity(&newTable, newCapacity);
  for(size_t i = 0; i < table->mappingCapacity; i++) {
    Mapping *m = VEC_GET(&table->mappings, i, Mapping);
    if(m->existent) {
      putTable(&newTable, m->key, m->keylen, m->value, m->valuelen);
    }
  }
  freeTable(table);
  *table = newTable;
}

void freeTable(Table *table) {
  // Free all mappings
  for (size_t i = 0; i < lengthVector(&table->mappings); i++) {
    freeMapping(VEC_GET(&table->mappings, i, Mapping));
  }
  // Now free vector
  freeVector(&table->mappings);
}

void putTable(Table *table, void *key, size_t keylen, void *value, size_t valuelen) {
  //TODO detect when to resize table

  uint32_t attempt = 0;
  // Linear probing algorithm
  while(true) {
    // Calculate index, increment attempt
    size_t index = hashTable(key, keylen, table->mappingCapacity, attempt);
    Mapping* m = VEC_GET(&table->mappings, index, Mapping);
    // If the mapping does not exist yet
    if(!m->existent) {
      initMapping(m, key, keylen, value, valuelen);
      return;
    }
    else { 
      // If the mapping has the right key
      if(keylen == m->keylen && memcmp(m->key, key, keylen) == 0) {
        if(valuelen != m->valuelen) {
          m->value = realloc(m->value, valuelen);
          m->valuelen = valuelen;
        }
        // Update mapping
        memcpy(m->value, value, valuelen);
        return;
      }
      else {
        // Just continue with next attempt
        attempt++;
      }
    }
  }
}

// TODO adapt to hash
void delTable(Table *table, void *key, size_t keylen) {
  for (size_t i = 0; i < lengthVector(&table->mappings); i++) {
    Mapping *m = VEC_GET(&table->mappings, i, Mapping);
    // If the keys match
    if (m->keylen == keylen && memcmp(m->key, key, keylen)) {
      VEC_REM(&table->mappings, i, Mapping);
      return;
    }
  }
}

// TODO adapt to hash
void getTable(Table *table, void *key, void *value, size_t keylen,
    size_t valuelen) {
  for (size_t i = 0; i < lengthVector(&table->mappings); i++) {
    Mapping *m = VEC_GET(&table->mappings, i, Mapping);
    // If the keys match
    if (m->keylen == keylen && memcmp(m->key, key, keylen)) {
      // Copy valuelen values to the given value
      memcpy(value, m->value, valuelen);
      return;
    }
  }
}

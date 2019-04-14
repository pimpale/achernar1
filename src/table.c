#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "hash.h"
#include "table.h"
#include "vector.h"

#define LOAD_FACTOR 0.2f
#define INITIAL_CAPACITY 256
#define EXPAND_FACTOR 2.0f

/* hash function using linear probing */
size_t hashTable(void *data, size_t datalen, size_t bucketCount,
                 uint32_t attempt);

size_t hashTable(void *data, size_t datalen, size_t bucketCount,
                 uint32_t attempt) {
  return ((simpleHash(data, datalen) + attempt) % bucketCount);
}

typedef struct {
  void *key;
  size_t keylen;
  void *value;
  size_t valuelen;
  bool existent;
} Mapping;

/* Initializes a mapping data structure */
void initMapping(Mapping *mapping, void *key, size_t keylen, void *value,
                 size_t valuelen);

/* Frees a mapping data structure */
void freeMapping(Mapping *mapping);

void initMapping(Mapping *mapping, void *key, size_t keylen, void *value,
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
  if (mapping->existent) {
    mapping->existent = false;
    free(mapping->key);
    free(mapping->value);
  }
}

void updateMappingValue(Mapping *mapping, void* value, size_t valuelen);
void updateMappingValue(Mapping *mapping, void* value, size_t valuelen) {
  if(mapping->valuelen != valuelen) {
    mapping->value = realloc(mapping->value, valuelen);
    mapping->valuelen = valuelen;
  }
  memcpy(mapping->value, value, valuelen);
}


size_t hashMappingTable(Mapping *mapping, size_t bucketCount, uint32_t attempt);
size_t hashMappingTable(Mapping *mapping, size_t bucketCount,
                        uint32_t attempt) {
  return (hashTable(mapping->key, mapping->keylen, bucketCount, attempt));
}

float currentLoadTable(Table *table);
float currentLoadTable(Table *table) {
  return (((float)table->mappingCount) / ((float)table->mappingCapacity));
}

void initTable(Table *table) { initTableCapacity(table, INITIAL_CAPACITY); }

void initTableCapacity(Table *table, size_t capacity) {
  table->mappingCount = 0;
  table->mappingCapacity = capacity;
  // Initialize vector
  initVector(&table->mappings);
  // Make space in vector for mappings
  pushVector(&table->mappings, capacity * sizeof(Mapping));
}

// Creates new table, inserts old stuff, deletes this table. Expensive, avoid
// this operation
void resizeTable(Table *table, size_t newCapacity) {
  Table newTable;
  initTableCapacity(&newTable, newCapacity);
  for (size_t i = 0; i < table->mappingCapacity; i++) {
    Mapping *m = VEC_GET(&table->mappings, i, Mapping);
    if (m->existent) {
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

size_t getMappingIndexTable(Table *table, void *key, size_t keylen);

size_t getMappingIndexTable(Table *table, void *key, size_t keylen) {
  uint32_t attempt = 0;
  // Linear probing algorithm
  while (true) {
    // Calculate index, increment attempt
    size_t index = hashTable(key, keylen, table->mappingCapacity, attempt);
    Mapping *m = VEC_GET(&table->mappings, index, Mapping);
    // If the mapping does not exist yet
    if (!m->existent) {
      return (index);
    }
    // If the mapping exists
    else {
      // If the keys match
      if (keylen == m->keylen && memcmp(m->key, key, keylen) == 0) {
        return (index);
      } else {
        // Once we've iterated through all possibilities
        if (attempt > table->mappingCapacity) {
          FATAL("Table lookup failed");
        } else {
          // Just continue with next attempt
          attempt++;
        }
      }
    }
  }
}

void putTable(Table *table, void *key, size_t keylen, void *value,
              size_t valuelen) {
  size_t index = getMappingIndexTable(table, key, keylen);
  Mapping *m = VEC_GET(&table->mappings, index, Mapping);
  // If m exists, just update it
  if (m->existent) {
    updateMappingValue(m,  value, valuelen);
  } else {
    // Create a new mapping and update the mapping count
    initMapping(m, key, keylen, value, valuelen);
    table->mappingCount++;
  }

  // If the load on table is greater than what it should be
  if (currentLoadTable(table) > LOAD_FACTOR) {
    // expand the size of this table
    resizeTable(table, (size_t)((float)table->mappingCapacity * EXPAND_FACTOR));
  }
  return;
}

void delTable(Table *table, void *key, size_t keylen) {
  size_t index = getMappingIndexTable(table, key, keylen);
  Mapping *m = VEC_GET(&table->mappings, index, Mapping);
  // If a mapping exists, free it
  if (m->existent) {
    freeMapping(m);
  }
}

size_t getValueLengthTable(Table* table, void *key, size_t keylen) {
  size_t index = getMappingIndexTable(table, key, keylen);
  Mapping *m = VEC_GET(&table->mappings, index, Mapping);
  // If m exists, return the valuelen, otherwise return 0
  return (m->existent ? m->valuelen : 0);
}

// TODO adapt to hash
void getTable(Table *table, void *key, size_t keylen, void *value,
              size_t valuelen) {
  size_t index = getMappingIndexTable(table, key, keylen);
  Mapping *m = VEC_GET(&table->mappings, index, Mapping);
  if (m->existent) {
    memcpy(value, m->value, valuelen);
  }
  return;
}


#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>
#include <string.h>
#include "vector.h"

// Do not manually modify
typedef struct {
  // The mappings in the table
  Vector mappings;
  // Number of mappings in mappings
  size_t mappingCount;
  // Total number of spots for mappings
  size_t mappingCapacity;
  // Ideal ratio of mappingCount to mappingCapacity 
  float loadFactor
} Table;

/* Initialize the table */
void initTable(Table *table);
/* Initialize the table with a default capacity */
void initTableCapacity(Table *table, size_t capacity);
/* Delete the table */
void freeTable(Table *table);

/* Puts a Mapping in the table, creating one if it doesnt exist. If value is null, will set valuelen*/
void putTable(Table *table, void *key, size_t keylen, void *value, size_t valuelen);

/* Retrieves the value of the table given the key
 * If the value is found, the */
void getTable(Table *table, void *key, void *value, size_t keylen,
              size_t valuelen);
/* Deletes the Mapping given by key */
void delTable(Table *table, void *key, size_t keylen);

#endif

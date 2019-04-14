#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <stddef.h>
#include "vector.h"

// Do not manually modify
typedef struct {
  // The mappings in the table
  Vector mappings;
  // Number of mappings in mappings
  size_t mappingCount;
  // Total number of spots for mappings
  size_t mappingCapacity;
} Table;

/* Initialize the table */
void initTable(Table *table);
/* Initialize the table with a default capacity */
void initTableCapacity(Table *table, size_t capacity);
/* Delete the table */
void freeTable(Table *table);

/* Puts a Mapping in the table, creating one if it doesnt exist. If value is null, will set valuelen*/
void putTable(Table *table, void *key, size_t keylen, void *value, size_t valuelen);

/* Returns the size of the value */
size_t getValueLengthTable(Table* table, void *key, size_t keylen);

/* Copies bytes of the value into value for the corresponding key. Caller must ensure via getValueLengthTable that it is the correct size */
void getTable(Table *table, void *key, size_t keylen, void *value,
              size_t valuelen);
/* Deletes the Mapping given by key */
void delTable(Table *table, void *key, size_t keylen);

/* Determines the current load of the table */
float currentLoadTable(Table* table);

#endif

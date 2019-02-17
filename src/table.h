#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>
#include <string.h>

// TODO make table not a linkedlist

struct Mapping {
  void *key;
  size_t keylen;
  void *value;
  size_t valuelen;

  struct Mapping *next;
};

typedef struct {
  struct Mapping *first;
} Table;

/* Initialize the table */
void initTable(Table *table);
/* Delete the table */
void freeTable(Table *table);

/* Puts a Mapping in the table */
void putTable(Table *table, void *key, void *value, size_t keylen,
              size_t valuelen);
/* Retrieves the value of the table given the key
 * If the value is found, the */
void getTable(Table *table, void *key, void *value, size_t keylen,
              size_t *valuelen);
/* Deletes the Mapping given by key */
void delTable(Table *table, void *key, size_t keylen);

#endif

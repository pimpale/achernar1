#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <string.h>

// TODO make table not a linkedlist

struct Tableping {
  void *key;
  size_t keylen;
  void *value;
  size_t valuelen;

  struct Tableping *next;
};

typedef struct {
  struct Tableping *first;
} Table;

/* Initialize the table */
void initTable(Table *table);
/* Delete the table */
void freeTable(Table *table);

/* Puts a Tableping in the table */
void putTable(Table *table, void *key, void *value, size_t keylen, size_t valuelen);
/* Retrieves the value of the table given the key
 * If the value is found, the */
void getTable(Table *table, void *key, void *value, size_t keylen, size_t *valuelen);
/* Deletes the Tableping given by key */
void delTable(Table *table, void *key, size_t keylen);

void Tableping_init(struct Tableping *tableping, void *key, void *value,
                  size_t keylen, size_t valuelen) {
  tableping->keylen = keylen;
  tableping->valuelen = valuelen;
  tableping->key = malloc(keylen);
  tableping->value = malloc(valuelen);
  memcpy(tableping->key, key, keylen);
  memcpy(tableping->value, value, valuelen);
  tableping->next = NULL;
}

void Tableping_free(struct Tableping *tableping) {
  free(tableping->key);
  free(tableping->value);
}

void initTable(Table *table) { table->first = NULL; }

void freeTable(Table *table) {
  struct Tableping *tableping = table->first;
  // Free each tableping
  while (tableping != NULL) {
    struct Tableping *next_tableping = tableping->next;
    Tableping_free(tableping);
    tableping = next_tableping;
  }
}

void putTable(Table *table, void *key, void *value, size_t keylen,
             size_t valuelen) {
  //Delete old tableping
  delTable(table, key, keylen);

  // Create new tableping
  struct Tableping *new_tableping = malloc(sizeof(struct Tableping));
  Tableping_init(new_tableping, key, value, keylen, valuelen);

  // Append to last part of list
  if (table->first != NULL) {
    struct Tableping *tableping = table->first;
    while (tableping->next != NULL) {
      tableping = tableping->next;
    }
    tableping->next = new_tableping;
  } else {
    table->first = new_tableping;
  }
}

void delTable(Table *table, void *key, size_t keylen) {
  struct Tableping *tableping = table->first;
  struct Tableping *previous = NULL;
  while (tableping != NULL) {
    if (keylen == tableping->keylen && !memcmp(tableping->key, key, tableping->keylen)) {
      if (tableping->next != NULL) {
        if (previous != NULL) {
          previous->next = tableping->next;
        }
      } else {
        if (previous != NULL) {
          previous->next = NULL;
        }
      }
      // if it is the first element in the list
      if (tableping == table->first) {
        table->first = NULL;
      }
      Tableping_free(tableping);
      return;
    }
    previous = tableping;
    tableping = tableping->next;
  }
}


void getTable(Table *table, void *key, void *value, size_t keylen,
             size_t *valuelen) {
  struct Tableping *tableping = table->first;
  while (tableping != NULL) {
    if (!memcmp(tableping->key, key, tableping->keylen)) {
      memcpy(value, tableping->value, tableping->valuelen);
      *valuelen = tableping->valuelen;
      return;
    }
    tableping = tableping->next;
  }
  *valuelen = 0;
}

#endif

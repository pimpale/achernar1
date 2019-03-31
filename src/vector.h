#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

#define INITIAL_CAPACITY 1000
#define LOAD_FACTOR 0.75

typedef struct {
  uint8_t *data;
  size_t length;
  size_t capacity;
} Vector;

void initVector(Vector *vector);
void freeVector(Vector *vector);


void push(Vector *vector, uint8_t data);
uint8_t pop(Vector *vector);

void pushData(Vector *vector, void *data, size_t len);
void popData(Vector *vector, void *data, size_t len);

#endif

#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

typedef struct {
  // Do not realloc data
  void *data;
  // Do not manually modify length or capacity
  size_t length;
  size_t capacity;
} Vector;

// Creates empty vector
void initVector(Vector *vector);
// Frees vector
void freeVector(Vector *vector);

void* insertVector(Vector *vector, size_t loc, size_t len);
void removeVector(Vector *vector, size_t loc, size_t len);

void* pushVector(Vector *vector, size_t len);
void popVector(Vector *vector, void *data, size_t len);

#endif

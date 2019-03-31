#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "vector.h"

//The initial capacity of the vector
#define INITIAL_CAPACITY 1000
// The percent it will increase when out of room MUST BE POSITIVE
// Ex. 0.5 -> 50% expansion each time the limit is hit 
#define LOAD_FACTOR 0.75 

void setSizeVector(Vector *vector, size_t size);
void resizeVector(Vector *vector, size_t size);

/* Sets the size of the vector */
void setSizeVector(Vector *vector, size_t size) {
  vector->data = realloc(vector->data, size);
  vector->capacity = size;
}

/* Resizes the vector in order to fit an element of this size in */
void resizeVector(Vector *vector, size_t size) {
  /* This is the new size of the vector if we used the loadFactor */
  size_t newCapacity = (size_t) ((vector->capacity+size) * (1 + LOAD_FACTOR));
  setSizeVector(vector, newCapacity);
}


void initVector(Vector *vector) {
  vector->data = NULL;
  vector->length = 0;
  resizeVector(vector, INITIAL_CAPACITY);
}

void freeVector(Vector *vector) {
  free(vector->data);
  vector->data = NULL;
}

void push(Vector *vector, uint8_t data) {
  if (vector->length == vector->capacity) {
    FATAL("vector overflow");
  }
  vector->data[vector->length++] = data;
}

uint8_t pop(Vector *vector) {
  if (vector->length == 0) {
    FATAL("vector underflow");
  }
  return (vector->data[--(vector->length)]);
}

void pushData(Vector *vector, void *data, size_t len) {
  if (vector->length + len >= vector->capacity) {
    FATAL("vector overflow");
  }
  memmove(&(vector->data[vector->length]), data, len);
  vector->length += len;
}

void popData(Vector *vector, void *data, size_t len) {
  if (len > vector->length) {
    FATAL("vector underflow");
  }
  vector->length -= len;
  memmove(data, &(vector->data[vector->length]), len);
}


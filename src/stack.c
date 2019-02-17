#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "constants.h"

#define INITIAL_CAPACITY 1000
#define LOAD_FACTOR 0.75


void resizeStack(Stack *stack, size_t size) {
  stack->data = realloc(stack->data, size);
  stack->capacity = size;
}

void initStack(Stack *stack) {
  stack->data = NULL;
  stack->stackpos = 0;
  resizeStack(stack, INITIAL_CAPACITY);
}

void freeStack(Stack *stack) {
  free(stack->data);
  stack->data = NULL;
}

void push(Stack *stack, uint8_t data) {
  if (stack->stackpos == stack->capacity) {
    FATAL("stack overflow");
  }
  stack->data[stack->stackpos++] = data;
}

uint8_t pop(Stack *stack) {
  if (stack->stackpos == 0) {
    FATAL("stack underflow");
  }
  return (stack->data[--(stack->stackpos)]);
}

void pushData(Stack *stack, void *data, size_t len) {
  if (stack->stackpos + len >= stack->capacity) {
    FATAL("stack overflow");
  }
  memmove(&(stack->data[stack->stackpos]), data, len);
  stack->stackpos += len;
}

void popData(Stack *stack, void *data, size_t len) {
  if (len > stack->stackpos) {
    FATAL("stack underflow");
  }
  stack->stackpos -= len;
  memmove(data, &(stack->data[stack->stackpos]), len);
}


#ifndef STACK_H
#define STACK_H

#include <stdint.h>

#define INITIAL_CAPACITY 1000
#define LOAD_FACTOR 0.75

typedef struct {
  uint8_t *data;
  size_t stackpos;
  size_t capacity;
} Stack;

void initStack(Stack *stack);
void freeStack(Stack *stack);

void resizeStack(Stack *stack, size_t size);

void push(Stack *stack, uint8_t data);
uint8_t pop(Stack *stack);

void pushData(Stack *stack, void *data, size_t len);
void popData(Stack *stack, void *data, size_t len);

#endif

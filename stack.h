#include <stdint.h>

typedef struct {
  uint8_t* ptr;
  size_t stackpos;
  size_t maxlen;
} Stack;

void initStack(Stack* stack);
void deleteStack(Stack* stack);

void push(Stack *stack, uint8_t data);
uint8_t pop(Stack *stack);

void pushData(Stack *stack, void* data, size_t len);
void popData(Stack *stack, void* data, size_t len);



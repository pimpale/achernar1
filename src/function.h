
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>

#include "stack.h"
#include "table.h"

#define FUNCTION_TYPE_NATIVE 1
#define FUNCTION_TYPE_FORTH 2
#define FUNCTION_TYPE_FILESYSTEM 3

typedef struct {
  void (*nativeFunPointer)(Stack *, Table *, Table *);
  char *body;
  FILE *file;
  uint64_t funType;
} Function;

void initPrelude(Table *funtab);
void freePrelude(Table *funtab);

#endif

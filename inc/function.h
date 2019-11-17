
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>

#include "table.h"
#include "vector.h"

typedef enum {
  FUNCTION_TYPE_NATIVE,
  FUNCTION_TYPE_FORTH,
  FUNCTION_TYPE_FILESYSTEM
} FunctionType;

typedef struct {
  FunctionType funType;
  void (*nativeFunPointer)(Vector *, Table *, Table *);
  FILE *file;
  char *body;
  size_t bodyLength;
} Function;

void initPrelude(Table *funtab);
void freePrelude(Table *funtab);

#endif

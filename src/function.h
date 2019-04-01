
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
  void (*nativeFunPointer)(Vector *, Table *, Table *);
  FILE *file;
  FunctionType funType;
  size_t bodyLength;
  char *body;
} Function;

void initPrelude(Table *funtab);

#endif

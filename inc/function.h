
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
#include <stdio.h>

#include "table.h"
#include "vector.h"

typedef enum {
  FUNCTION_TYPE_NATIVE,
  FUNCTION_TYPE_FORTH,
} FunctionType;

typedef struct {
  FunctionType funType;
  void (*nativeFunPointer)(Vector *, Table *, Table *);
  char *body;
  size_t bodyLength;
} Function;

// Creates the minimal environment required to run this lang
void initPrelude(Table *funtab);

// Function creation
void initNativeFunction(Function *fun, void (*funPtr)(Vector *, Table *, Table *));
void initForthFunction(Function *fun, char *body);
// Run function
void executeFunction(Function *fun, Vector *stack, Table *funtab, Table *vartab);
// Function deletion
void freeFunction(Function *fun);

#endif

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
#include <stdlib.h>

#include "table.h"
#include "stack.h"

#define FUNCTION_TYPE_NATIVE 1
#define FUNCTION_TYPE_FORTH 2
#define FUNCTION_TYPE_FILESYSTEM 3

typedef struct {
  void (*nativeFunPointer)(Stack*, Table*, Table*);
  char* body;
  FILE* file;
  uint64_t funType;
} Function;

void initPrelude(Table* funtab);
void freePrelude(Table* funtab);

/*** IMPLEMENTATION **/

void initNativeFunction(Function* fun, void (*funPtr)(Stack*, Table*, Table*)) {
  fun->funType= FUNCTION_TYPE_NATIVE;
  fun->nativeFunPointer = funPtr;
  fun->body = NULL;
  fun->file = NULL;
}

void initForthFunction(Function* fun, char* body) {
  fun->funType= FUNCTION_TYPE_FORTH;
  size_t len = strlen(body);
  fun->body = malloc(len+1);
  strcpy(body, fun->body);
  fun->file = NULL;
  fun->nativeFunPointer = NULL;
}

void initFileFunction(Function* fun, FILE* file) {
  fun->funType= FUNCTION_TYPE_FILESYSTEM;
  fun->body = NULL;
  fun->nativeFunPointer = NULL;
  fun->file = file;
}

void freeFunction(Function *fun) {
  free(fun->body);
  fun->body = NULL;
  fun->nativeFunPointer = NULL;
  fun->file = NULL;
}

// Variable manipulation function
void mkvar(Stack* stack, Table* funtab, Table* vartab);
void delvar(Stack* stack, Table* funtab, Table* vartab);
void getvar(Stack* stack, Table* funtab, Table* vartab);
void putvar(Stack* stack, Table* funtab, Table* vartab);

// Function manipulation function
void mkfun(Stack* stack, Table* funtab, Table* vartab);
void delfun(Stack* stack, Table* funtab, Table* vartab);

// Looping and conditionals
void evalif(Stack* stack, Table* funtab, Table* vartab);
void loop(Stack* stack, Table* funtab, Table* vartab);

#define MATH_DEFINE_TYPE(type)                  \
  void add_##type(Stack* stack, Table* funtab, Table* vartab) { \
    type a, b, sum;                                         \
    popData(stack, &a, sizeof(a));                          \
    popData(stack, &b, sizeof(b));                          \
    sum = a + b;                                            \
    pushData(stack, &sum, sizeof(sum));                     \
  }                                                         \
  void sub_##type(Stack* stack, Table* funtab, Table* vartab) { \
    type a, b, sum;                                         \
    popData(stack, &a, sizeof(a));                          \
    popData(stack, &b, sizeof(b));                          \
    sum = a - b;                                            \
    pushData(stack, &sum, sizeof(sum));                     \
  }                                                         \
  void mul_##type(Stack* stack, Table* funtab, Table* vartab) { \
    type a, b, sum;                                         \
    popData(stack, &a, sizeof(a));                          \
    popData(stack, &b, sizeof(b));                          \
    sum = a * b;                                            \
    pushData(stack, &sum, sizeof(sum));                     \
  }                                                         \
  void div_##type(Stack* stack, Table* funtab, Table* vartab) { \
    type a, b, sum;                                         \
    popData(stack, &a, sizeof(a));                          \
    popData(stack, &b, sizeof(b));                          \
    sum = a / b;                                            \
    pushData(stack, &sum, sizeof(sum));                     \
  }

MATH_DEFINE_TYPE(char)
MATH_DEFINE_TYPE(long)
MATH_DEFINE_TYPE(double)

#undef MATH_DEFINE_TYPE

#define NATIVE_FUNCTION_PUT(funName, stringLiteral)                      \
  do {                                                                   \
    Function f;                                                          \
    initNativeFunction(&f, &(funName));                                  \
    putTable(funtab, stringLiteral, &f, strlen(stringLiteral), sizeof(f)); \
  } while (0)

#define MATH_TYPE_PUT(type)                    \
  do {                                         \
    NATIVE_FUNCTION_PUT(add_##type, "+##type"); \
    NATIVE_FUNCTION_PUT(add_##type, "+##type"); \
    NATIVE_FUNCTION_PUT(add_##type, "+##type"); \
    NATIVE_FUNCTION_PUT(add_##type, "+##type"); \
  } while (0)


void initPrelude(Table* funtab) {

  MATH_TYPE_PUT(char);
  MATH_TYPE_PUT(long);
  MATH_TYPE_PUT(double);
  
  NATIVE_FUNCTION_PUT(mkvar, "mkvar");
  NATIVE_FUNCTION_PUT(delvar, "delvar");
  NATIVE_FUNCTION_PUT(getvar, "getvar");
  NATIVE_FUNCTION_PUT(putvar, "putvar");
  NATIVE_FUNCTION_PUT(mkfun, "mkfun");
  NATIVE_FUNCTION_PUT(delfun, "delfun");
  NATIVE_FUNCTION_PUT(evalif, "evalif");
  NATIVE_FUNCTION_PUT(loop, "loop");

}

#undef MATH_TYPE_PUT
#undef NATIVE_FUNCTION_PUT

void freePrelude(Table* funtab) {

}
#endif

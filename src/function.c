#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "table.h"
#include "vector.h"

#include "function.h"

void initNativeFunction(Function *fun,
                        void (*funPtr)(Vector *, Table *, Table *)) {
  fun->funType = FUNCTION_TYPE_NATIVE;
  fun->nativeFunPointer = funPtr;
  fun->body = NULL;
  fun->file = NULL;
  fun->bodyLength = 0;
}

void initForthFunction(Function *fun, char *body) {
  fun->funType = FUNCTION_TYPE_FORTH;
  fun->bodyLength = strlen(body) + 1;
  fun->body = malloc(fun->bodyLength);
  strcpy(body, fun->body);
  fun->file = NULL;
  fun->nativeFunPointer = NULL;
}

void initFileFunction(Function *fun, FILE *file) {
  fun->funType = FUNCTION_TYPE_FILESYSTEM;
  fun->body = NULL;
  fun->nativeFunPointer = NULL;
  fun->file = file;
  fun->bodyLength = 0;
}

void freeFunction(Function *fun) {
  free(fun->body);
  free(fun->body);
  fun->nativeFunPointer = NULL;
  fun->file = NULL;
  fun->bodyLength = 0;
}

// Variable manipulation function

void mkvar(Vector *stack, Table *funtab, Table *vartab);
void delvar(Vector *stack, Table *funtab, Table *vartab);
void getvar(Vector *stack, Table *funtab, Table *vartab);
void putvar(Vector *stack, Table *funtab, Table *vartab);

void mkvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);
  size_t varsize;
  popVector(stack, &varsize, sizeof(varsize));

  char *vardata = malloc(varsize);

  size_t namesize;
  popVector(stack, &namesize, sizeof(namesize));
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  putTable(vartab, name, vardata, namesize, varsize);
  free(name);
  free(vardata);
}

void delvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);
  size_t namesize;
  popVector(stack, &namesize, sizeof(namesize));
  char *name = malloc(namesize);
  popVector(stack, name, namesize);
  delTable(vartab, name, namesize);
  free(name);
}

void getvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);
  size_t namesize;
  popVector(stack, &namesize, sizeof(namesize));
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  size_t varsize;
  getTable(vartab, name, NULL, namesize, &varsize);

  char *vardata = malloc(varsize);
  getTable(vartab, name, vardata, namesize, &varsize);
  memcpy(pushVector(stack, varsize), vardata, varsize);
  free(name);
  free(vardata);
}

// Pops the data off the stack and into the variable
void putvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);
  // First we find the name
  // Find the name size
  size_t namesize;
  VEC_POP(stack, &namesize, size_t);
  // Pop the name off of the stack
  char *name = malloc(namesize);
  popVector(stack, name, namesize);
}
// Function manipulation function
void mkfun(Vector *stack, Table *funtab, Table *vartab);
void delfun(Vector *stack, Table *funtab, Table *vartab);
void runfun(Vector *stack, Table *funtab, Table *vartab);

// Looping and conditionals
void evalif(Vector *stack, Table *funtab, Table *vartab);
void loop(Vector *stack, Table *funtab, Table *vartab);

#define OPERATOR_DEFINE_TYPE(type, operatorName, operator)                  \
  void operatorName##_##type(Vector *stack, Table *funtab, Table *vartab);  \
  void operatorName##_##type(Vector *stack, Table *funtab, Table *vartab) { \
    UNUSED(vartab);                                                         \
    UNUSED(funtab);                                                         \
    type arg1, arg2, ret;                                                   \
    popVector(stack, &arg1, sizeof(arg1));                                  \
    popVector(stack, &arg2, sizeof(arg2));                                  \
    ret = arg1 operator arg2;                                               \
    *((type *)pushVector(stack, sizeof(ret))) = ret;                        \
  }

#define MATH_DEFINE_TYPE(type)       \
  OPERATOR_DEFINE_TYPE(type, add, +) \
  OPERATOR_DEFINE_TYPE(type, sub, -) \
  OPERATOR_DEFINE_TYPE(type, mul, *) \
  OPERATOR_DEFINE_TYPE(type, div, /)

#define NATIVE_FUNCTION_PUT(funName, stringLiteral)                        \
  do {                                                                     \
    Function f;                                                            \
    initNativeFunction(&f, &(funName));                                    \
    putTable(funtab, stringLiteral, &f, strlen(stringLiteral), sizeof(f)); \
  } while (0)

#define MATH_TYPE_PUT(type, name)               \
  do {                                          \
    NATIVE_FUNCTION_PUT(add_##type, "+##name"); \
    NATIVE_FUNCTION_PUT(sub_##type, "-##name"); \
    NATIVE_FUNCTION_PUT(div_##type, "/##name"); \
    NATIVE_FUNCTION_PUT(mul_##type, "*##name"); \
  } while (0)

#define NATIVE_FUNCTION_DEL(stringLiteral)                  \
  do {                                                      \
    delTable(funtab, stringLiteral, strlen(stringLiteral)); \
  } while (0)

#define MATH_TYPE_DEL(name)         \
  do {                              \
    NATIVE_FUNCTION_DEL("+##name"); \
    NATIVE_FUNCTION_DEL("-##name"); \
    NATIVE_FUNCTION_DEL("/##name"); \
    NATIVE_FUNCTION_DEL("*##name"); \
  } while (0)

MATH_DEFINE_TYPE(uint8_t)
MATH_DEFINE_TYPE(uint64_t)
MATH_DEFINE_TYPE(double)

void initPrelude(Table *funtab) {
  MATH_TYPE_PUT(uint8_t, u8);
  MATH_TYPE_PUT(uint64_t, u64);
  MATH_TYPE_PUT(double, f64);

  NATIVE_FUNCTION_PUT(mkvar, "mkvar");
  NATIVE_FUNCTION_PUT(delvar, "delvar");
  NATIVE_FUNCTION_PUT(getvar, "getvar");
  NATIVE_FUNCTION_PUT(putvar, "putvar");
  NATIVE_FUNCTION_PUT(mkfun, "mkfun");
  NATIVE_FUNCTION_PUT(delfun, "delfun");
  NATIVE_FUNCTION_PUT(evalif, "evalif");
  NATIVE_FUNCTION_PUT(loop, "loop");
}

void freePrelude(Table *funtab) {
  MATH_TYPE_DEL(u8);
  MATH_TYPE_DEL(u64);
  MATH_TYPE_DEL(f64);

  NATIVE_FUNCTION_DEL("mkvar");
  NATIVE_FUNCTION_DEL("delvar");
  NATIVE_FUNCTION_DEL("getvar");
  NATIVE_FUNCTION_DEL("putvar");
  NATIVE_FUNCTION_DEL("mkfun");
  NATIVE_FUNCTION_DEL("delfun");
  NATIVE_FUNCTION_DEL("evalif");
  NATIVE_FUNCTION_DEL("loop");
}

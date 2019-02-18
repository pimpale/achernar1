#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "stack.h"
#include "table.h"

#include "function.h"

void initNativeFunction(Function *fun,
                        void (*funPtr)(Stack *, Table *, Table *)) {
  fun->funType = FUNCTION_TYPE_NATIVE;
  fun->nativeFunPointer = funPtr;
  fun->body = NULL;
  fun->file = NULL;
}

void initForthFunction(Function *fun, char *body) {
  fun->funType = FUNCTION_TYPE_FORTH;
  size_t len = strlen(body);
  fun->body = malloc(len + 1);
  strcpy(body, fun->body);
  fun->file = NULL;
  fun->nativeFunPointer = NULL;
}

void initFileFunction(Function *fun, FILE *file) {
  fun->funType = FUNCTION_TYPE_FILESYSTEM;
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
void mkvar(Stack *stack, Table *funtab, Table *vartab) {
  size_t varsize;
  popData(stack, &varsize, sizeof(varsize));

  char *vardata = malloc(varsize);

  size_t namesize;
  popData(stack, &namesize, sizeof(namesize));
  char *name = malloc(namesize);
  popData(stack, name, namesize);

  putTable(vartab, name, vardata, namesize, varsize);
  free(name);
  free(vardata);
}

void delvar(Stack *stack, Table *funtab, Table *vartab) {
  size_t namesize;
  popData(stack, &namesize, sizeof(namesize));
  char *name = malloc(namesize);
  popData(stack, name, namesize);
  delTable(vartab, name, namesize);
  free(name);
}

void getvar(Stack *stack, Table *funtab, Table *vartab) {
  size_t namesize;
  popData(stack, &namesize, sizeof(namesize));
  char *name = malloc(namesize);
  popData(stack, name, namesize);

  size_t varsize;
  getTable(vartab, name, NULL, namesize, &varsize);

  char* vardata = malloc(varsize);
  getTable(vartab, name, vardata, namesize, &varsize);
  pushData(stack, vardata, varsize);
  free(name);
  free(vardata);
}
void putvar(Stack *stack, Table *funtab, Table *vartab) {
  //TODO
}
// Function manipulation function
void mkfun(Stack *stack, Table *funtab, Table *vartab);
void delfun(Stack *stack, Table *funtab, Table *vartab);
void runfun(Stack *stack, Table *funtab, Table *vartab);

// Looping and conditionals
void evalif(Stack *stack, Table *funtab, Table *vartab);
void loop(Stack *stack, Table *funtab, Table *vartab);

#define OPERATOR_DEFINE_TYPE(type, operatorName, operator)                 \
  void operatorName##_##type(Stack *stack, Table *funtab, Table *vartab);  \
  void operatorName##_##type(Stack *stack, Table *funtab, Table *vartab) { \
    UNUSED(vartab);                                                        \
    UNUSED(funtab);                                                        \
    type arg1, arg2, ret;                                                  \
    popData(stack, &arg1, sizeof(arg1));                                   \
    popData(stack, &arg2, sizeof(arg2));                                   \
    ret = arg1 operator arg2;                                              \
    pushData(stack, &ret, sizeof(ret));                                    \
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
    NATIVE_FUNCTION_PUT(add_##type, "-##name"); \
    NATIVE_FUNCTION_PUT(add_##type, "/##name"); \
    NATIVE_FUNCTION_PUT(add_##type, "*##name"); \
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

void freePrelude(Table *funtab) {}

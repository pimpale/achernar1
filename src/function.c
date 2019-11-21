
#include "function.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "parse.h"
#include "parseable.h"
#include "table.h"
#include "vector.h"

void initNativeFunction(Function *fun,
                        void (*funPtr)(Vector *, Table *, Table *)) {
  fun->funType = FUNCTION_TYPE_NATIVE;
  fun->nativeFunPointer = funPtr;
  fun->body = NULL;
  fun->bodyLength = 0;
}

void initForthFunction(Function *fun, char *body) {
  fun->funType = FUNCTION_TYPE_FORTH;
  fun->bodyLength = strlen(body) + 1;
  fun->body = malloc(fun->bodyLength);
  strcpy(body, fun->body);
  fun->nativeFunPointer = NULL;
}

void executeFunction(Function *fun, Vector *stack, Table *funtab,
                     Table *vartab) {
  switch (fun->funType) {
    case FUNCTION_TYPE_NATIVE: {
      fun->nativeFunPointer(stack, funtab, vartab);
      break;
    }
    case FUNCTION_TYPE_FORTH: {
      Parseable bodyParser;
      initParseableMemory(&bodyParser, fun->body, fun->bodyLength);
      parse(&bodyParser, stack, funtab, vartab);
      freeParseable(&bodyParser);
      break;
    }
  }
}

void freeFunction(Function *fun) {
  free(fun->body);
  fun->nativeFunPointer = NULL;
  fun->bodyLength = 0;
}

// Variable manipulation function

void mkvar(Vector *stack, Table *funtab, Table *vartab);
void delvar(Vector *stack, Table *funtab, Table *vartab);
void getvar(Vector *stack, Table *funtab, Table *vartab);
void putvar(Vector *stack, Table *funtab, Table *vartab);

// Creates a variable that can be used to store things
// 10 (my-value) mkvar
// This would create a space of 10 bytes assigned to the name my-value
void mkvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);

  // Get the name
  size_t namesize;
  VEC_POP(stack, &namesize, size_t);
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  // Allocate space for the variable
  uint8_t varsize;
  VEC_POP(stack, &varsize, uint8_t);
  char *vardata = malloc(varsize);

  // If a variable with this name already exists, we must free it
  if (getValueLengthTable(vartab, name, namesize) != 0) {
    delTable(vartab, name, namesize);
  }

  putTable(vartab, name, namesize, vardata, varsize);
  free(name);
  free(vardata);
}

// Deletes a variable's space and unassociates the name
// (my-value) delvar
void delvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);
  size_t namesize;
  VEC_POP(stack, &namesize, size_t);
  char *name = malloc(namesize);
  popVector(stack, name, namesize);
  delTable(vartab, name, namesize);
  free(name);
}

// pushes the value of a variable's space onto the stack
// (my-value) getvar
void getvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);

  // Get name
  size_t namesize;
  VEC_POP(stack, &namesize, size_t);
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  // Get size of variable
  size_t varsize = getValueLengthTable(vartab, name, namesize);

  if (varsize == 0) {
    printf("function: getvar: variable name `%s` not defined\n", name);
    FATAL("FUNCTION USAGE ERROR");
  }

  // We push the value of the variable to the vector
  getTable(vartab, name, namesize, pushVector(stack, varsize), varsize);

  // Now free the name
  free(name);
}

// Pops the data off the stack and into the variable
// Ex: 1 2 + (sum) putvar
// Thie example would put 3 into sum
void putvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);
  // First we find the name of variable
  // Find the name size
  size_t namesize;
  VEC_POP(stack, &namesize, size_t);
  // Pop the name off of the stack
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  // Then we check size of this variable
  size_t varsize = getValueLengthTable(vartab, name, namesize);

  if (varsize == 0) {
    printf("function: putvar: variable name `%s` not defined\n", name);
    FATAL("FUNCTION USAGE ERROR");
  }

  // Now we pop however many bytes of the thing from the vector to the table
  uint8_t *vardata = getVector(stack, lengthVector(stack) - varsize);
  putTable(vartab, name, namesize, vardata, varsize);
  // Now we delete from the stack
  popVector(stack, NULL, varsize);

  // free
  free(name);
}

// Function manipulation function
void mkfun(Vector *stack, Table *funtab, Table *vartab);
void delfun(Vector *stack, Table *funtab, Table *vartab);

// Pops the data off the stack and into the variable
// Ex: ((hello) println) (say-hello) mkfun
void mkfun(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(vartab);

  // First we find the name
  // Find the name size
  size_t namesize;
  VEC_POP(stack, &namesize, size_t);
  // Pop the name off of the stack
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  // Find the body
  // Find the body size
  size_t bodysize;
  VEC_POP(stack, &bodysize, size_t);
  // Pop the body off the stack
  char *body = malloc(bodysize);
  popVector(stack, body, bodysize);

  // If a function by this name already exists, we must free it
  if (getValueLengthTable(funtab, name, namesize) != 0) {
    // First we must free the Function within the table
    Function funToDelete;
    getTable(funtab, name, namesize, &funToDelete, sizeof(Function));
    freeFunction(&funToDelete);
    // Now we delete from the table itself
    delTable(funtab, name, namesize);
  }

  // Now create function and put it in funtab
  Function fun;
  initForthFunction(&fun, body);

  // Add this to the table
  putTable(funtab, name, namesize, &fun, sizeof(Function));

  // Free memory
  free(name);
  free(body);
}

// Deletes a function
// Ex: (println) delfun
// This would delete the function println
void delfun(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(vartab);

  // First we find the name
  // Find the name size
  size_t namesize;
  VEC_POP(stack, &namesize, size_t);
  // Pop the name off of the stack
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  // If a function by this name already exists, we must free it
  if (getValueLengthTable(funtab, name, namesize) != 0) {
    // First we must free the Function within the table
    Function funToDelete;
    getTable(funtab, name, namesize, &funToDelete, sizeof(Function));
    freeFunction(&funToDelete);
    // Now we delete from the table itself
    delTable(funtab, name, namesize);
  }
}

// Looping and conditionals
void eval(Vector *stack, Table *funtab, Table *vartab);
void evalif(Vector *stack, Table *funtab, Table *vartab);
void loop(Vector *stack, Table *funtab, Table *vartab);
void println(Vector *stack, Table *funtab, Table *vartab);

// Evaluates the string unconditionally
// Ex: (1 2 +) eval
void eval(Vector *stack, Table *funtab, Table *vartab) {
  // Find the string size
  size_t stringsize;
  VEC_POP(stack, &stringsize, size_t);
  // Pop the string off of the stack
  char *string = malloc(stringsize);
  popVector(stack, string, stringsize);

  // Now we make a parseable of the string and parse
  Parseable parseable;
  initParseableMemory(&parseable, string, stringsize);

  // Parse it in this context
  parse(&parseable, stack, funtab, vartab);

  freeParseable(&parseable);
  free(string);
}

// Evaluates if the value is not 0
// Ex: 1 ((hello) println) evalif
void evalif(Vector *stack, Table *funtab, Table *vartab) {
  // Find the string size
  size_t stringsize;
  VEC_POP(stack, &stringsize, size_t);
  // Pop the string off of the stack
  char *string = malloc(stringsize);
  popVector(stack, string, stringsize);

  // Pop the value
  uint8_t value;
  VEC_POP(stack, &value, uint8_t);

  // If it's not false
  if (value != 0) {
    // Now we make a parseable of the string and parse
    Parseable parseable;
    initParseableMemory(&parseable, string, stringsize);

    // Parse it in this context
    parse(&parseable, stack, funtab, vartab);
    freeParseable(&parseable);
  }
  free(string);
}

// Evals until the value on the stack is 0
// Format: (body to run) (body to check if true) loop
// Ex:
//    # Say hello 10 times
//    10
//    (
//      (hello) println
//      1 -
//      dupu8
//    ) loop
//
// This example creates a variable with one byte of space, sets it to 10
// Each loop it decrements and then finishes
void loop(Vector *stack, Table *funtab, Table *vartab) {
  // Get the body
  // Find the body size
  size_t bodysize;
  VEC_POP(stack, &bodysize, size_t);
  // Pop the body off the stack
  char *body = malloc(bodysize);
  popVector(stack, body, bodysize);

  // Now make parseable
  while (true) {
    uint8_t result;
    VEC_POP(stack, &result, uint8_t);

    if (result == 0) {
      break;
    }
    // Evaluate the body, if the final result is not 0, keep going
    // bit
    Parseable bodyParseable;
    initParseableMemory(&bodyParseable, body, bodysize);
    // Parse it in this context
    parse(&bodyParseable, stack, funtab, vartab);
    freeParseable(&bodyParseable);
  }

  free(body);
}

// Prints string to standard output
// Ex: (hello world!) println
// This example would print "hello world!" to the output, with a newline
void println(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);
  UNUSED(vartab);
  // Find the string size
  size_t stringsize;
  VEC_POP(stack, &stringsize, size_t);
  // Pop the string off of the stack
  char *string = malloc(stringsize);
  popVector(stack, string, stringsize);

  // Print it
  puts(string);
  // Free
  free(string);
}

/* Function that takes in two args returns one */
#define DEFINE_ARG2_RET1_NATIVE_FUN(type, identifier, operation1)         \
  void identifier##_##type(Vector *stack, Table *funtab, Table *vartab);  \
  void identifier##_##type(Vector *stack, Table *funtab, Table *vartab) { \
    UNUSED(vartab);                                                       \
    UNUSED(funtab);                                                       \
    type arg1, arg2, ret1;                                                \
    popVector(stack, &arg1, sizeof(arg1));                                \
    popVector(stack, &arg2, sizeof(arg2));                                \
    ret1 = operation1;                                                     \
    *((type *)pushVector(stack, sizeof(ret1))) = ret1;                     \
  }

#define DEFINE_TYPE(type)                                        \
  /* Define Math Functions */                                    \
  DEFINE_ARG2_RET1_NATIVE_FUN(type, add, arg1 + arg2)            \
  DEFINE_ARG2_RET1_NATIVE_FUN(type, sub, arg1 - arg2)            \
  DEFINE_ARG2_RET1_NATIVE_FUN(type, mul, arg1 *arg2)             \
  DEFINE_ARG2_RET1_NATIVE_FUN(type, div, arg1 / arg2)            \
  /* Define dup, drop, and swp */                                \
  void dup_##type(Vector *stack, Table *funtab, Table *vartab);  \
  void drop_##type(Vector *stack, Table *funtab, Table *vartab); \
  void swp_##type(Vector *stack, Table *funtab, Table *vartab);  \
  void dup_##type(Vector *stack, Table *funtab, Table *vartab) { \
    UNUSED(funtab);                                              \
    UNUSED(vartab);                                              \
    type arg1, ret1, ret2;                                       \
    popVector(stack, &arg1, sizeof(arg1));                       \
    ret1 = arg1;                                                 \
    ret2 = arg1;                                                 \
    *((type *)pushVector(stack, sizeof(ret1))) = ret1;           \
    *((type *)pushVector(stack, sizeof(ret2))) = ret2;           \
  }                                                              \
  void drop_##type(Vector *stack, Table *funtab, Table *vartab) \
  {                                                              \
    UNUSED(funtab);                                              \
    UNUSED(vartab);                                              \
    type arg1;                                                   \
    popVector(stack, &arg1, sizeof(arg1));                       \
  }                                                              \
  void swp_##type(Vector *stack, Table *funtab, Table *vartab) { \
    UNUSED(funtab);                                              \
    UNUSED(vartab);                                              \
    type arg1, arg2, ret1, ret2;                                 \
    popVector(stack, &arg1, sizeof(arg1));                       \
    popVector(stack, &arg2, sizeof(arg2));                       \
    ret1 = arg1;                                                 \
    ret2 = arg1;                                                 \
    *((type *)pushVector(stack, sizeof(ret1))) = ret1;           \
    *((type *)pushVector(stack, sizeof(ret2))) = ret2;           \
  }

#define NATIVE_FUNCTION_PUT(funName, stringLiteral)                     \
  do {                                                                  \
    char *string = stringLiteral;                                       \
    Function f;                                                         \
    initNativeFunction(&f, &(funName));                                 \
    putTable(funtab, string, strlen(string) + 1, &f, sizeof(Function)); \
  } while (0)

#define PUT_TYPE(type, name)                        \
  do {                                              \
    NATIVE_FUNCTION_PUT(add_##type, "+" #name);     \
    NATIVE_FUNCTION_PUT(sub_##type, "-" #name);     \
    NATIVE_FUNCTION_PUT(mul_##type, "*" #name);     \
    NATIVE_FUNCTION_PUT(div_##type, "/" #name);     \
    NATIVE_FUNCTION_PUT(dup_##type, "dup" #name);   \
    NATIVE_FUNCTION_PUT(drop_##type, "drop" #name); \
    NATIVE_FUNCTION_PUT(swp_##type, "swp" #name);   \
  } while (0)

DEFINE_TYPE(uint8_t)
DEFINE_TYPE(uint64_t)
DEFINE_TYPE(double)

void initPrelude(Table *funtab) {
  PUT_TYPE(uint8_t, u8);
  PUT_TYPE(uint64_t, u64);
  PUT_TYPE(double, f64);

  NATIVE_FUNCTION_PUT(mkvar, "mkvar");
  NATIVE_FUNCTION_PUT(delvar, "delvar");
  NATIVE_FUNCTION_PUT(getvar, "getvar");
  NATIVE_FUNCTION_PUT(putvar, "putvar");
  NATIVE_FUNCTION_PUT(mkfun, "mkfun");
  NATIVE_FUNCTION_PUT(delfun, "delfun");
  NATIVE_FUNCTION_PUT(eval, "eval");
  NATIVE_FUNCTION_PUT(evalif, "evalif");
  NATIVE_FUNCTION_PUT(loop, "loop");
  NATIVE_FUNCTION_PUT(println, "println");
}

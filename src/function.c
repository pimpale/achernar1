#include "function.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "table.h"
#include "vector.h"

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

// Creates a variable that can be used to store things
// 10 (my-value) mkvar
// This would create a space of 10 bytes assigned to the name my-value
void mkvar(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);

  // Allocate space for the variable
  size_t varsize;
  popVector(stack, &varsize, sizeof(varsize));
  char *vardata = malloc(varsize);

  // Get the name
  size_t namesize;
  popVector(stack, &namesize, sizeof(namesize));
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  // If a variable with this name already exists, we must free it
  if(getValueLengthTable(vartab, name, namesize) != 0) {
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
  popVector(stack, &namesize, sizeof(namesize));
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
  popVector(stack, &namesize, sizeof(namesize));
  char *name = malloc(namesize);
  popVector(stack, name, namesize);

  // Get size of variable
  size_t varsize = getValueLengthTable(vartab, name, namesize);

  if(varsize == 0) {
    // TODO error handling we want to make this crash
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

  if(varsize == 0) {
    // TODO handle error
  }

  // Now we pop however many bytes of the thing from the vector to the table
  uint8_t *vardata = getVector(stack, lengthVector(stack) - varsize);
  putTable(vartab, name, namesize, vardata, varsize);
  // Now we delete from the stack
  popVector(stack, varsize);

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
  if(getValueLengthTable(funtab, name, namesize) != 0) {
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
  putTable(funtab, name, &fun, namesize, sizeof(fun));

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
  if(getValueLengthTable(funtab, name, namesize) != 0) {
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

// Evaluates the string unconditionally
// Ex: (1 2 +) eval
void eval(Vector *stack, Table *funtab, Table *vartab) {
  // Find the string size
  size_t stringsize;
  VEC_POP(stack, &stringsize, size_t);
  // Pop the string off of the stack
  char *string = malloc(stringsize);
  popVector(stack, string, namesize);

  // Now we make a parseable of the string and parse
  Parseable parseable;
  initParseableMemory(&parseable, string, stringsize);

  // Parse it in this context
  parse(&parseable, stack, funtab, vartab);

  freeParseable(&parseable);
  free(string);
}

// Evaluates if the value is not 0
// Ex: ((hello) println) 1 evalif
void evalif(Vector *stack, Table *funtab, Table *vartab) {
  // Pop the value
  uint8_t value;
  VEC_POP(stack, &value, sizeof(value));

  // If it's not false
  if(value != 0) {
    eval(stack, funtab, vartab);
  }
}

// Evals the body forever until the value of the second clause is equal to zero
// Format: (body to run) (body to check if true) loop
// Ex:
//    # Say hello 10 times
//    1 (counter) mkvar
//    10 (counter) putvar
//    (
//      (hello) println
//      (counter) getvar 1 - putvar
//    ) (counter) loop
//
// This example creates a variable with one byte of space, sets it to 10
// Each loop it decrements and then finishes
void loop(Vector *stack, Table *funtab, Table *vartab) {

  // Get the evaluator
  size_t evaluatorsize;
  VEC_POP(stack, &evaluatorsize, size_t);
  // Pop the name off of the stack
  char *evaluator = malloc(evaluatorsize);
  popVector(stack, evaluator, evaluatorsize);

  // Get the body
  // Find the body size
  size_t bodysize;
  VEC_POP(stack, &bodysize, size_t);
  // Pop the body off the stack
  char *body = malloc(bodysize);
  popVector(stack, body, bodysize);

  // Now make parseable

  while(true) {

    // Evaluate the evaluator, if the final result is not 0, evaluate the next bit
    Parseable evaluatorParseable;
    initParseableMemory(&evaluatorParseable, evaluator, evaluatorsize);
    // Parse it in this context
    parse(&evaluatorParseable, stack, funtab, vartab);
    freeParseable(&evaluatorParseable);

    uint8_t result;
    VEC_POP(stack, &result, uint8_t);

    if(result == 0) {
      break;
    }
    
    Parseable bodyParseable;
    initParseableMemory(&bodyParseable, body, bodysize);
    parse(&bodyParseable, stack, funtab, vartab);
    freeParseable(
  }

  free(body);
  free(evaluator);
}



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

#define NATIVE_FUNCTION_PUT(funName, stringLiteral)          \
  do {                                                       \
    char *string = stringLiteral;                            \
    Function f;                                              \
    initNativeFunction(&f, &(funName));                      \
    putTable(funtab, string, &f, strlen(string), sizeof(f)); \
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


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

// Stack

static void hexDump(const char *desc, const void *addr, const size_t len) {
  size_t i;
  unsigned char buff[17];
  const unsigned char *pc = (const unsigned char *)addr;

  // Output description if given.
  if (desc != NULL) printf("%s:\n", desc);

  if (len == 0) {
    printf("  ZERO LENGTH\n");
    return;
  }

  // Process every byte in the data.
  for (i = 0; i < len; i++) {
    // Multiple of 16 means new line (with line offset).

    if ((i % 16) == 0) {
      // Just don't print ASCII for the zeroth line.
      if (i != 0) printf("  %s\n", buff);

      // Output the offset.
      printf("  %04zx ", i);
    }

    // Now the hex code for the specific character.
    printf(" %02x", pc[i]);

    // And store a printable ASCII character for later.
    if ((pc[i] < 0x20) || (pc[i] > 0x7e))
      buff[i % 16] = '.';
    else
      buff[i % 16] = pc[i];
    buff[(i % 16) + 1] = '\0';
  }

  // Pad out last line if not exactly 16 characters.
  while ((i % 16) != 0) {
    printf("   ");
    i++;
  }

  // And print the final ASCII bit.
  printf("  %s\n", buff);
}

// Stack Manipulation

static void dump(Vector *stack, Table *funtab, Table *vartab) {
  UNUSED(funtab);
  UNUSED(vartab);
  hexDump("stack", getVector(stack, 0), lengthVector(stack));
}

// Variable manipulation functions ====================================

// Creates a variable that can be used to store things
// 10 (my-value) mkvar
// This would create a space of 10 bytes assigned to the name my-value
static void mkvar(Vector *stack, Table *funtab, Table *vartab) {
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
static void delvar(Vector *stack, Table *funtab, Table *vartab) {
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
static void getvar(Vector *stack, Table *funtab, Table *vartab) {
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
static void putvar(Vector *stack, Table *funtab, Table *vartab) {
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

// Function manipulation functions ===============================================

// Pops the data off the stack and into the variable
// Ex: ((hello) println) (say-hello) mkfun
static void mkfun(Vector *stack, Table *funtab, Table *vartab) {
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
static void delfun(Vector *stack, Table *funtab, Table *vartab) {
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

// Looping and conditionals =====================================================

// Evaluates the string unconditionally
// Ex: (1 2 +) eval
static void eval(Vector *stack, Table *funtab, Table *vartab) {
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

// Evaluates first if the value is not 0, and the second one if so
// Ex: 1 ((hello) println) () ifthen
static void ifthen(Vector *stack, Table *funtab, Table *vartab) {

  // Find the else size
  size_t elsebodysize;
  VEC_POP(stack, &elsebodysize, size_t);
  // Pop the else off of the stack
  char *elsebody = malloc(elsebodysize);
  popVector(stack, elsebody, elsebodysize);

  // Find the if size
  size_t ifbodysize;
  VEC_POP(stack, &ifbodysize, size_t);
  // Pop the if off of the stack
  char *ifbody = malloc(ifbodysize);
  popVector(stack, ifbody, ifbodysize);


  // Pop the value
  uint8_t value;
  VEC_POP(stack, &value, uint8_t);

  // Now we make a parseable of the string and parse
  Parseable parseable;

  // Select the statement to evaluate
  if(value != 0) {
    // If condition true
    initParseableMemory(&parseable, ifbody, ifbodysize);
  } else {
    // If condition false
    initParseableMemory(&parseable, elsebody, elsebodysize);
  }

  // Parse it in this context
  parse(&parseable, stack, funtab, vartab);
  freeParseable(&parseable);

  free(ifbody);
  free(elsebody);
}

// Evals until the value on the stack is 0
// Format: (body to run) (body to check if true) loop
//
// Ex:
//    # Say hello 10 times
//    10
//    (
//      (hello) println
//      1 -u8
//      dupu8
//    ) loop
//
// This example creates a variable with one byte of space, sets it to 10
// Each loop it decrements and then finishes
static void loop(Vector *stack, Table *funtab, Table *vartab) {
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
static void println(Vector *stack, Table *funtab, Table *vartab) {
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
  static void identifier##_##type(Vector *stack, Table *funtab, Table *vartab) { \
    UNUSED(vartab);                                                       \
    UNUSED(funtab);                                                       \
    type arg1, arg2, ret1;                                                \
    popVector(stack, &arg1, sizeof(arg1));                                \
    popVector(stack, &arg2, sizeof(arg2));                                \
    ret1 = operation1;                                                    \
    *((type *)pushVector(stack, sizeof(ret1))) = ret1;                    \
  }

#define DEFINE_TYPE(type)                                         \
  /* Define Math Functions */                                     \
  DEFINE_ARG2_RET1_NATIVE_FUN(type, add, arg2 + arg1)             \
  DEFINE_ARG2_RET1_NATIVE_FUN(type, sub, arg2 - arg1)             \
  DEFINE_ARG2_RET1_NATIVE_FUN(type, mul, arg2 *arg1)              \
  DEFINE_ARG2_RET1_NATIVE_FUN(type, div, arg2 / arg1)             \
  /* Define dup, drop, and swp */                                 \
  static void dup_##type(Vector *stack, Table *funtab, Table *vartab) {  \
    UNUSED(funtab);                                               \
    UNUSED(vartab);                                               \
    type arg1, ret1, ret2;                                        \
    popVector(stack, &arg1, sizeof(arg1));                        \
    ret1 = arg1;                                                  \
    ret2 = arg1;                                                  \
    *((type *)pushVector(stack, sizeof(ret1))) = ret1;            \
    *((type *)pushVector(stack, sizeof(ret2))) = ret2;            \
  }                                                               \
  static void drop_##type(Vector *stack, Table *funtab, Table *vartab) { \
    UNUSED(funtab);                                               \
    UNUSED(vartab);                                               \
    type arg1;                                                    \
    popVector(stack, &arg1, sizeof(arg1));                        \
  }                                                               \
  static void swp_##type(Vector *stack, Table *funtab, Table *vartab) {  \
    UNUSED(funtab);                                               \
    UNUSED(vartab);                                               \
    type arg1, arg2, ret1, ret2;                                  \
    popVector(stack, &arg1, sizeof(arg1));                        \
    popVector(stack, &arg2, sizeof(arg2));                        \
    ret1 = arg1;                                                  \
    ret2 = arg2;                                                  \
    *((type *)pushVector(stack, sizeof(ret1))) = ret1;            \
    *((type *)pushVector(stack, sizeof(ret2))) = ret2;            \
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
  NATIVE_FUNCTION_PUT(ifthen, "ifthen");
  NATIVE_FUNCTION_PUT(loop, "loop");
  NATIVE_FUNCTION_PUT(println, "println");
  NATIVE_FUNCTION_PUT(dump, "dump");
}

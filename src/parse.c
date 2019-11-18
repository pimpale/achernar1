#include "parse.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


#include "function.h"
#include "constants.h"
#include "parseable.h"
#include "table.h"
#include "vector.h"

static void parseString(Parseable *stream, Vector *stack) {
  if (nextValue(stream) != '(') {
    FATAL("malformed string literal");
  }
  int32_t c;
  uint32_t depth = 1;
  uint32_t strlength = 0;
  while ((c = nextValue(stream)) != EOF) {
    if (strlength == UINT32_MAX) {
      FATAL("string literal out of bounds");
    } else if (c == '\\') {
      c = nextValue(stream);
      if (c == EOF) {
        break;
      }
    } else if (c == '(') {
      depth++;
      *VEC_PUSH(stack, uint8_t) = (uint8_t)c;
    } else if (c == ')') {
      depth--;
      if (depth == 0) {
        break;
      } else {
        *VEC_PUSH(stack, uint8_t) = (uint8_t)c;
      }
    } else {
      *VEC_PUSH(stack, uint8_t) = (uint8_t)c;
    }
    strlength++;
  }
  *VEC_PUSH(stack, uint8_t) = 0;  // signal end
  strlength++;
  *VEC_PUSH(stack, size_t) = strlength;  // push strlength
  printf("%zu\n", strlength);
}

// parse until space encountered, then push number.
// Numbers > 255 result in undefined behavior
static void parseNumber(Parseable *stream, Vector *stack) {
  int32_t c;
  size_t numBufPos = 0;
  char numBuf[NUMERICAL_LITERAL_MAX + 1];
  while ((c = nextValue(stream)) != EOF) {
    if (!isdigit(c) || numBufPos >= NUMERICAL_LITERAL_MAX) {
      break;
    } else {
      numBuf[numBufPos++] = (char)c;
    }
  }
  // terminate string
  numBuf[numBufPos] = '\0';
  int num = atoi(numBuf);
  if (num > UINT8_MAX || num < 0) {
    FATAL("numerical literal out of bounds");
  }
  *VEC_PUSH(stack, uint8_t) = (uint8_t)num;
}

static void parseFunction(Parseable *stream, Vector *stack, Table *funtab,
    Table *vartab) {
  UNUSED(stack);
  UNUSED(vartab);
  UNUSED(funtab);
  char functionBuf[FUNCTION_NAME_MAX + 1];
  size_t len = 0;
  int32_t c;
  while ((c = nextValue(stream)) != EOF && len <= FUNCTION_NAME_MAX) {
    if (isblank(c) || c == '\n') {
      break;
    } else {
      functionBuf[len++] = (char)c;
    }
  }
  functionBuf[len] = '\0';

  // Now eval function
  Function fun;
  if(getValueLengthTable(funtab, functionBuf, len+1) == 0) {
    FATAL("function not found");
  }
  getTable(funtab, functionBuf, len+1, &fun, sizeof(Function));
  executeFunction(&fun, stack, funtab, vartab);
}

// Parses stream until end
void parse(Parseable *stream, Vector *stack, Table *funtab, Table *vartab) {
  int32_t c;
  while ((c = peekValue(stream)) != EOF) {
    if (isblank(c) || c == '\n') {
      nextValue(stream);
    }
    else if (c == '(') {
      parseString(stream, stack);
    }
    else if (isdigit(c)) {
      parseNumber(stream, stack);
    }
    else {
      parseFunction(stream, stack, funtab, vartab);
    }
  }
}

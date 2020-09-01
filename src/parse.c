#include "parse.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "function.h"
#include "parseable.h"
#include "table.h"
#include "vector.h"

static void parseComment(Parseable *stream, Vector *stack) {
  UNUSED(stack);
  if (nextValue(stream) != '#') {
    FATAL("malformed comment");
  }
  int32_t c;
  while ((c = nextValue(stream)) != EOF) {
    if (c == '\n') {
      break;
    }
  }
}

static void parseString(Parseable *stream, Vector *stack) {
  if (nextValue(stream) != '(') {
    FATAL("malformed string literal");
  }
  int32_t c;
  uint32_t depth = 1;
  uint32_t strlength = 0;
  while ((c = nextValue(stream)) != EOF) {
    if (strlength == UINT32_MAX) {
      printf("parse: string length exceeds maximum value\n");
      FATAL("SYNTAX ERROR");
    } else if (c == '#') {
      backValue(stream);
      parseComment(stream, stack);
    } else if (c == '\\') {
      // Skip the backslash
      c = nextValue(stream);
      if (c == EOF) {
        break;
      } else if (c == 'n') {
        *VEC_PUSH(stack, uint8_t) = (uint8_t)'\n';
      } else if (c == '(') {
        *VEC_PUSH(stack, uint8_t) = (uint8_t)'(';
      } else if (c == ')') {
        *VEC_PUSH(stack, uint8_t) = (uint8_t)')';
      } else if (c == '#') {
        *VEC_PUSH(stack, uint8_t) = (uint8_t)'#';
      } else if (c == '\\') {
        *VEC_PUSH(stack, uint8_t) = (uint8_t)'\\';
      } else {
        printf("parse: unrecognized character after \\\n");
        FATAL("SYNTAX ERROR");
      }
      strlength++;
    } else if (c == '(') {
      depth++;
      *VEC_PUSH(stack, uint8_t) = (uint8_t)c;
      strlength++;
    } else if (c == ')') {
      depth--;
      if (depth == 0) {
        break;
      } else {
        *VEC_PUSH(stack, uint8_t) = (uint8_t)c;
      }
      strlength++;
    } else {
      *VEC_PUSH(stack, uint8_t) = (uint8_t)c;
      strlength++;
    }
  }
  *VEC_PUSH(stack, uint8_t) = 0;  // signal end
  strlength++;
  *VEC_PUSH(stack, size_t) = strlength;  // push strlength
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
    printf("parse: numerical value out of bounds\n");
    FATAL("SYNTAX ERROR");
  }
  *VEC_PUSH(stack, uint8_t) = (uint8_t)num;
}

static void parseFunction(Parseable *stream, Vector *stack, Table *funtab) {
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
  if (getValueLengthTable(funtab, functionBuf, len + 1) == 0) {
    printf("parse: unknown function: `%s`\n", functionBuf);
    FATAL("SYNTAX ERROR");
  }
  getTable(funtab, functionBuf, len + 1, &fun, sizeof(Function));
  executeFunction(&fun, stack, funtab);
}

// Parses stream until end
void parse(Parseable *stream, Vector *stack, Table *funtab) {
  int32_t c;
  while ((c = nextValue(stream)) != EOF) {
    // Unget c for the next function
    backValue(stream);
    if (isblank(c) || c == '\n') {
      nextValue(stream);
    } else if (c == '#') {
      parseComment(stream, stack);
    } else if (c == '(') {
      parseString(stream, stack);
    } else if (isdigit(c)) {
      parseNumber(stream, stack);
    } else {
      parseFunction(stream, stack, funtab);
    }
  }
}

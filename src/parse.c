#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "parse.h"
#include "parseable.h"
#include "vector.h"
#include "table.h"

/* Private Function Definitions */
void parseString(Parseable *stream, Vector *vector);
void parseNumber(Parseable *stream, Vector *vector);
void parseFunction(Parseable *stream, Vector *vector, Table *funtab,
                   Table *vartab);

void parseString(Parseable *stream, Vector *vector) {
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
      push(vector, (uint8_t)c);
    } else if (c == ')') {
      depth--;
      if (depth == 0) {
        break;
      } else {
        push(vector, (uint8_t)c);
      }
    } else {
      push(vector, (uint8_t)c);
    }
    strlength++;
  }
  push(vector, 0);  // signal end
  strlength++;
  pushData(vector, &strlength, sizeof(size_t));  // push strlength
}

// parse until space encountered, then push number.
// Numbers > 255 result in undefined behavior
void parseNumber(Parseable *stream, Vector *vector) {
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
  numBuf[numBufPos] = '\0';
  int num = atoi(numBuf);
  if (num > UINT8_MAX || num < 0) {
    FATAL("numerical literal out of bounds");
  }
  push(vector, (uint8_t)num);
}

void parseFunction(Parseable *stream, Vector *vector, Table *funtab,
                   Table *vartab) {
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
  // TODO make function stuff happen
}

// Parses stream until end
void parse(Parseable *stream, Vector *vector, Table *funtab, Table *vartab) {
  int32_t c;
  while ((c = nextValue(stream)) != EOF) {
    if (!isblank(c) && c != '\n') {
      backValue(stream);
      if (c == '(' || c == ')') {
        parseString(stream, vector);
      } else if (isdigit(c)) {
        parseNumber(stream, vector);
      } else {
        parseFunction(stream, vector, funtab, vartab);
      }
    }
  }
}

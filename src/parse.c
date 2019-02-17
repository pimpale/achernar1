#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <stdio.h>

#include "constants.h"
#include "stack.h"
#include "table.h"

#include "parse.h"

void parseString(FILE *stream, Stack *stack) {
  if (getc(stream) != '(') {
    FATAL("malformed string literal");
  }
  push(stack, 0); // signal beginning
  int32_t c;
  uint32_t depth = 1;
  uint32_t strlength = 1;
  while ((c = getc(stream)) != EOF) {
    if (strlength == UINT32_MAX) {
      FATAL("string literal out of bounds");
    } else if (c == '\\') {
      c = getc(stream);
      if (c == EOF) {
        break;
      }
    } else if (c == '(') {
      depth++;
      push(stack, (uint8_t)c);
    } else if (c == ')') {
      depth--;
      if (depth == 0) {
        break;
      } else {
        push(stack, (uint8_t)c);
      }
    } else {
      push(stack, (uint8_t)c);
    }
    strlength++;
  }
  push(stack, 0);                              // signal end
  pushData(stack, &strlength, sizeof(size_t)); // push strlength
}

// parse until space encountered, then push number.
// Numbers > 255 result in undefined behavior
void parseNumber(FILE *stream, Stack *stack) {
  int32_t c;
  size_t numBufPos = 0;
  char numBuf[NUMERICAL_LITERAL_MAX + 1];
  while ((c = getc(stream)) != EOF) {
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
  push(stack, (uint8_t)num);
}

void parseFunction(FILE *stream, Stack *stack, Table *funtab, Table *vartab) {
  char functionBuf[FUNCTION_NAME_MAX + 1];
  size_t len = 0;
  int32_t c;
  while ((c = getc(stream)) != EOF && len <= FUNCTION_NAME_MAX) {
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

void eval(char *str, Stack *stack, Table *funtab, Table *vartab) {
  FILE *stream = fmemopen(str, strlen(str), "r");
  parse(stream, stack, funtab, vartab);
  fclose(stream);
}

// Parses stream until end
void parse(FILE *stream, Stack *stack, Table *funtab, Table *vartab) {
  int32_t c;
  while ((c = getc(stream)) != EOF) {
    if (!isblank(c) && c != '\n') {
      ungetc(c, stream);
      if (c == '(' || c == ')') {
        parseString(stream, stack);
      } else if (isdigit(c)) {
        parseNumber(stream, stack);
      } else {
        parseFunction(stream, stack, funtab, vartab);
      }
    }
  }
}

int main() {
}
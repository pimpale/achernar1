#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VARIABLE_TABLE_MAX 10000
#define FUNCTION_TABLE_MAX 10000
#define STACK_MAX 10000

#define NUMERICAL_LITERAL_MAX 3
#define FUNCTION_NAME_MAX 32

typedef struct {
  void *ptr;
  char *name;
  size_t len;
} Variable;

typedef struct {
  char *func;
} Function;

Variable variableTable[VARIABLE_TABLE_MAX];
Function functionTable[FUNCTION_TABLE_MAX];

size_t stackpos = 0;
uint8_t stack[STACK_MAX];

void parseString(FILE *stream) {
  if (getc(stream) != '(') {
    FATAL("malformed string literal");
  }
  push(0); // signal beginning
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
      push((uint8_t)c);
    } else if (c == ')') {
      depth--;
      if (depth == 0) {
        break;
      } else {
        push((uint8_t)c);
      }
    } else {
      push((uint8_t)c);
    }
    strlength++;
  }
  push(0);                              // signal end
  pushData(&strlength, sizeof(size_t)); // push strlength (as an int)
}

// parse until space encountered, then push number.
// Numbers > 255 result in undefined behavior
void parseNumber(FILE *stream) {
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
  push((uint8_t)num);
}

void parse(FILE *stream);

void evalif() {
  if (pop()) {
    // allocate buffer to store string
    size_t size;
    popData(&size, sizeof(size_t));
    uint8_t *strbuf = malloc(size);
    popData(strbuf, size);
    pop(); // pop leading null byte
    FILE *stream = fmemopen(strbuf, size, "r");
    parse(stream);
    fclose(stream);
    free(strbuf);
  }
}

void parseFunction(FILE *stream) {
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
  if (!strcmp(functionBuf, "evalif")) {
    evalif();
  } else if (!strcmp(functionBuf, "mkvar")) {
    printstr();
  } else if (!strcmp(functionBuf, "delvar")) {
    repeat();
  } else {
    FILE *fp = fopen(functionBuf, "r");
    parse(fp);
    fclose(fp);
  }
}

// Parses stream until end
void parse(FILE *stream) {
  int32_t c;
  while ((c = getc(stream)) != EOF) {
    if (!isblank(c) && c != '\n') {
      ungetc(c, stream);
      if (c == '(' || c == ')') { // String literal
        parseString(stream);
      } else if (isdigit(c)) { // Numerical literal
        parseNumber(stream);
      } else {
        parseFunction(stream);
      }
    }
  }
}

int main() {
  FILE *in = fopen("/dev/stdin", "r");
  parse(in);
  fclose(in);
}

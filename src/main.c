#include <stdio.h>

#include "function.h"
#include "vector.h"
#include "table.h"
#include "parseable.h"
#include "parse.h"

int main() {
  Vector stack;
  Table funtab;
  Table vartab;
  Parseable input;

  initVector(&stack);
  initTable(&funtab);
  initTable(&vartab);
  initParseableFile(&input, stdin);

  parse(&input, &stack, &funtab, &vartab);

  freeParseable(&input);
  freeVector(&stack);
  freeTable(&funtab);
  freeTable(&vartab);
}

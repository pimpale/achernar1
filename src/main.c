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

  // Instantiate environment
  initVector(&stack);
  initTable(&funtab);
  initTable(&vartab);
  // Read from stdin
  initParseableFile(&input, stdin);

  // Populate default function
  initPrelude(&funtab);

  // Parse
  parse(&input, &stack, &funtab, &vartab);

  // Free resources
  freeParseable(&input);
  freeVector(&stack);
  freeTable(&funtab);
  freeTable(&vartab);
}

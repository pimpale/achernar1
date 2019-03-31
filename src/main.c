#include <stdio.h>

#include "function.h"
#include "vector.h"
#include "table.h"
#include "parseable.h"
#include "parse.h"

int main() {
  Vector vector;
  Table funtab;
  Table vartab;
  Parseable input;

  initVector(&vector);
  initTable(&funtab);
  initTable(&vartab);
  initParseableFile(&input, stdin);

  parse(&input, &vector, &funtab, &vartab);

  freeParseable(&input);
  freeVector(&vector);
  freeTable(&funtab);
  freeTable(&vartab);
}

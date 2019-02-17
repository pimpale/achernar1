#include <stdio.h>

#include "function.h"
#include "parse.h"
#include "stack.h"
#include "table.h"

int main() {
  Stack stack;
  Table funtab;
  Table vartab;

  initStack(&stack);
  initTable(&funtab);
  initTable(&vartab);

  parse(stdin, &stack, &funtab, &vartab);

  freeStack(&stack);
  freeTable(&funtab);
  freeTable(&vartab);
}

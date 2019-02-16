#include <stdio.h>

#include "map.h"
#include "stack.h"
#include "parse.h"

int main() {
  Stack stack;
  Map funtab;
  Map vartab;

  initStack(&stack);
  initMap(&funtab);
  initMap(&vartab);

  parse(stdin, &stack, &funtab, &vartab);

  freeStack(&stack);
  freeMap(&funtab);
  freeMap(&vartab);
}

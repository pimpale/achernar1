#include <stdio.h>

#include "function.h"
#include "vector.h"
#include "table.h"
#include "parseable.h"
#include "parse.h"

int main(int argc, char** argv) {
  Vector stack;
  Table funtab;
  Table vartab;

  // Instantiate environment
  initVector(&stack);
  initTable(&funtab);
  initTable(&vartab);

  // Populate default function
  initPrelude(&funtab);

  if(argc > 1) {
    // Parse each file
    for(int i = 1; i < argc; i++) {
      Parseable fileParseable;
      FILE* file = fopen(argv[i], "r");
      if(file != NULL) {
        initParseableFile(&fileParseable, file);
        // Parse
        parse(&fileParseable, &stack, &funtab, &vartab);
        freeParseable(&fileParseable);
      }
      else {
        fprintf(stderr, "ERROR: could not open file %s\n", argv[i]);
      }
    }
  }
  else {
    // Read from stdin
    Parseable input;
    initParseableFile(&input, stdin);
    // Parse
    parse(&input, &stack, &funtab, &vartab);
    freeParseable(&input);
  }

  // Free resources
  freeVector(&stack);
  freeTable(&funtab);
  freeTable(&vartab);
}

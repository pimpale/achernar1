#ifndef PARSEABLE_H
#define PARSEABLE_H

#include <stdio.h>
#include <stdint.h>


typedef enum {
  PARSEABLE_BACKING_MEMORY,
  PARSEABLE_BACKING_FILE,
} ParseableBacking;

/* Do not manually modify any of these values  */

typedef struct {
  /* The backing data structure */
  ParseableBacking backing;
  FILE* file;
  char* memory;
  size_t len;
  /* Caches the value of the last char fetched for ungetc */
  int lastVal;
  /* location in file */
  size_t loc;
} Parseable;


void initParseableFile(Parseable* parseable, FILE* fp);
void initParseableMemory(Parseable* parseable, char* ptr, size_t len);

int32_t nextValue(Parseable* p);
int32_t peekValue(Parseable* p);

void freeParseable(Parseable* p);

#endif /* PARSEABLE_H */

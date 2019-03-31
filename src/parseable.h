#ifndef PARSEABLE_H
#define PARSEABLE_H

#include <stdio.h>
#include <stdint.h>


typedef enum {
  PARSEABLE_BACKING_MEMORY_MALLOCED,
  PARSEABLE_BACKING_FILE,
  PARSEABLE_BACKING_MEMORY_STACK
} ParseableBacking;

/* Do not manually modify any of these values  */ 

typedef struct {
  FILE* file;
  ParseableBacking backing; 
  int lastVal; /* Caches the value of the last char fetched for ungetc */
  size_t loc;
  size_t len;
  char* memory;
} Parseable;


void initParseableFile(Parseable* parseable, FILE* fp);
void initParseableMemory_m(Parseable* parseable, char* ptr, size_t len);
void initParseableMemory_s(Parseable* parseable, char* ptr, size_t len);

int32_t nextValue(Parseable* p);
void backValue(Parseable* p);

void freeParseable(Parseable* p);

#endif /* PARSEABLE_H */

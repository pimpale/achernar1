#include <stdint.h>
#include <stdio.h>

#include "constants.h"
#include "parseable.h"

void initParseableFile(Parseable* parseable, FILE* fp) {
  parseable->backing = PARSEABLE_BACKING_FILE;
  parseable->file = fp;
  parseable->memory = NULL;
  parseable->len = 0;
  parseable->loc = 0;
}
void initParseableMemory_m(Parseable* parseable, char* ptr, size_t len) {
  parseable->backing = PARSEABLE_BACKING_MEMORY_MALLOCED;
  parseable->file = NULL;
  parseable->memory = ptr;
  parseable->len = len;
  parseable->loc = 0;
}
void initParseableMemory_s(Parseable* parseable, char* ptr, size_t len) {
  parseable->backing = PARSEABLE_BACKING_MEMORY_STACK;
  parseable->file = NULL;
  parseable->memory = ptr;
  parseable->len = len;
  parseable->loc = 0;
}

int32_t nextValue(Parseable* p) {
  int32_t nextValue;
  switch (p->backing) {
    case PARSEABLE_BACKING_MEMORY_STACK:  // Fall through
    case PARSEABLE_BACKING_MEMORY_MALLOCED: {
      if (p->loc >= p->len) {
        nextValue = EOF;
      } else {
        // Return the element at the location, and increment location
        nextValue  = (p->memory[(p->loc)++]);
      }
      break;
    }
    case PARSEABLE_BACKING_FILE: {
      p->lastVal = getc(p->file);
      nextValue = (p->lastVal);
      break;
    }
  }
  return nextValue;
}

void backValue(Parseable* p) {
  switch (p->backing) {
    case PARSEABLE_BACKING_MEMORY_STACK: // Fall through
    case PARSEABLE_BACKING_MEMORY_MALLOCED:
      {
        if(p->loc == 0) {
          p->loc--;
          return;
        }
    case PARSEABLE_BACKING_FILE:
        ungetc(p->lastVal, p->file);
        return;
      }
  }
}

void freeParseable(Parseable* p) {
  //do nothing for now
  UNUSED(p);
}

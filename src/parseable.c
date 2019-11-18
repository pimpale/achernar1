#include <stdint.h>
#include <stdio.h>

#include "constants.h"
#include "parseable.h"

void initParseableFile(Parseable* parseable, FILE* fp) {
  // TODO seek file to beginning?
  parseable->backing = PARSEABLE_BACKING_FILE;
  parseable->file = fp;
  parseable->memory = NULL;
  parseable->len = 0;
  parseable->loc = 0;
}
void initParseableMemory(Parseable* parseable, char* ptr, size_t len) {
  parseable->backing = PARSEABLE_BACKING_MEMORY;
  parseable->file = NULL;
  parseable->memory = ptr;
  parseable->len = len;
  parseable->loc = 0;
}

int32_t nextValue(Parseable* p) {
  int32_t nextValue;
  switch (p->backing) {
    case PARSEABLE_BACKING_MEMORY: {
      if (p->loc + 1>= p->len) {
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

int32_t peekValue(Parseable* p) {
  int32_t peekValue;
  switch (p->backing) {
    case PARSEABLE_BACKING_MEMORY: {
      if (p->loc +1 >= p->len) {
        peekValue = EOF;
      } else {
        // Return the element at the location, and increment location
        peekValue  = (p->memory[p->loc]);
      }
      break;
    }
    case PARSEABLE_BACKING_FILE: {
      int32_t c = getc(p->file);
      ungetc(c, p->file);
      p->lastVal = c;
      peekValue = (p->lastVal);
      break;
    }
  }
  return peekValue;
}



void freeParseable(Parseable* p) {
  //do nothing for now
  UNUSED(p);
}

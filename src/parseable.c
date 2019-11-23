#include "parseable.h"

#include <stdint.h>
#include <stdio.h>

#include "constants.h"

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
      if (p->loc + 1 >= p->len) {
        nextValue = EOF;
      } else {
        // Return the element at the location, and increment location
        nextValue = (p->memory[(p->loc)++]);
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
    case PARSEABLE_BACKING_MEMORY: {
      if (p->loc == 0) {
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
  // do nothing for now
  UNUSED(p);
}

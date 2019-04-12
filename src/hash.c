#include <stdint.h>
#include <stdlib.h>

#include "hash.h"

uint64_t simpleHash(void *data, size_t datalen){
  uint64_t hash =0;
  uint8_t* bytes = data;
  for (size_t i = 0; i < datalen; i++){
    hash = hash*bytes[i] + bytes[i] + i;
  }
  return (hash);
}

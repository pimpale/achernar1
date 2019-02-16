#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>
#include <stdlib.h>

#define FATAL(x)                                                               \
  do {                                                                         \
    fprintf(stderr, x);                                                        \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#endif

#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>

#include "vector.h"
#include "table.h"

#define FUNCTION_NAME_MAX 10
#define NUMERICAL_LITERAL_MAX 10

void parse(Parseable *stream, Vector *vector, Table *funtab, Table *vartab);

#endif /* PARSE_H */

#ifndef PARSE_H
#define PARSE_H

#include "parseable.h"
#include "table.h"
#include "vector.h"

#define FUNCTION_NAME_MAX 10
#define NUMERICAL_LITERAL_MAX 10

void parse(Parseable *stream, Vector *stack, Table *funtab);

#endif /* PARSE_H */

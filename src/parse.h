#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>

#include "stack.h"
#include "table.h"

#define FUNCTION_NAME_MAX 10
#define NUMERICAL_LITERAL_MAX 10

void eval(char *str, Stack *stack, Table *funtab, Table *vartab);
void parse(FILE *file, Stack *stack, Table *funtab, Table *vartab);
void parseString(FILE *stream, Stack *stack);
void parseNumber(FILE *stream, Stack *stack);
void parseFunction(FILE *stream, Stack *stack, Table *funtab, Table *vartab);

#endif

#ifndef ASSEMBLE_UTIL_H
#define ASSEMBLE_UTIL_H

#include "symbol_table.h"
#include <stdio.h>

typedef struct {
  char **lines;
  uint fileLinesCount;
} file_lines;

typedef struct {
  char *key;
  int value;
} pair_t;

extern file_lines *scanFile(FILE *armFile, symbol_table *symbolTable);

#endif

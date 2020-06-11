#ifndef FILE_LINES_H
#define FILE_LINES_H

#include "../common/util.h"

/* Structure that stores the ARM file's lines as an array of strings */
typedef struct {
  char **lines;
  uint lineCount, maxLines;
} file_lines;

extern void addLine(file_lines *fl, char *line);
extern void addLines(file_lines *fl, char **lines, uint n);
extern void printFileLines(file_lines *fileLines);
extern file_lines *newFileLines();
extern void freeFileLines(file_lines *fileLines);

#endif

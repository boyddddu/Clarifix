#ifndef PARSER_H
#define PARSER_H

/* Parser interface */
void advance(void);
void program(void);

/* Syntax error counter */
extern int syntaxErrorCount;

/* Shared function for printing analysis messages */
void tracePrint(const char *format, ...);

#include <stdio.h>

/* Write the collected syntax errors to the report file */
void writeSyntaxErrors(FILE *report_fp);

#endif
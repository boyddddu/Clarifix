#ifndef PARSER_H
#define PARSER_H

/* Parser interface */
void advance(void);
void program(void);

/* Syntax error counter */
extern int syntaxErrorCount;

/* Shared function for printing analysis messages */
void tracePrint(const char *format, ...);
void traceRule(const char *rule, int depth, int entering);
void beginExpressionTrace(int depth);
void endExpressionTrace(int hadError);

#include <stdio.h>

/* Write the collected syntax errors to the report file */
void writeSyntaxErrors(FILE *report_fp);

#endif
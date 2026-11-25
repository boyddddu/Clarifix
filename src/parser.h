#ifndef PARSER_H
#define PARSER_H

/* Parser interface */
void advance(void);
void program(void);

/* Syntax error counter */
extern int syntaxErrorCount;

#endif
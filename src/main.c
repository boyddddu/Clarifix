#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

/* Global declarations */
/* Variables */
int charClass;
char lexeme[100];
char commentLexeme[1000];
int commentLen = 0;
int nextChar;
int lexLen;

/*pang track ing indentation*/
int currentIndent = 0;
int atLineStart = 1;
int indentStack[100];
int indentTop = 0;
int errorCount = 0;
int lexemeOverflow = 0;
int indentationChecked = 0;
int indentationError = 0;

int hasStatement = 0; //tracking para sa noise and comments since ignored sila ng parser

int lineNumber = 1;
int tokenLine = 1;

int token;
char *nextToken;

#define MAX_TOKENS 1000

int tokenLines[MAX_TOKENS];
char tokenNames[MAX_TOKENS][30];
char tokenLexemes[MAX_TOKENS][1000];

int storedTokenCount = 0;

/* Maximum number of lexical errors to store */
#define MAX_ERRORS 100

/* Arrays for storing error information */
int errorLines[MAX_ERRORS];
char errorMessages[MAX_ERRORS][200];

int storedErrorCount = 0;

FILE *in_fp;

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
char *lex();
int isClxFile(char filename[]);

char *checkKeyword();


/* Character classes */
#define LETTER 0
#define DIGIT 1
#define SPECIAL 2
#define NEW_LINE 3
#define UNKNOWN 99

/* Token names */
#define CONST_INT "CONST_INT"
#define IDENTIFIER "IDENTIFIER"
#define ASSIGN "ASSIGN"
#define ADD "ADD"
#define SUBTRACT "SUBTRACT"
#define MULTIPLY "MULTIPLY"
#define DIVIDE "DIVIDE"
#define LEFT_PAREN "LEFT_PAREN"
#define RIGHT_PAREN "RIGHT_PAREN"
#define ERROR_TOKEN "ERROR"

/* Keywords tokens*/
#define K_set "K_set"
#define K_show "K_show"
#define K_ask "K_ask"
#define K_if "K_if"
#define K_elif "K_elif"
#define K_else "K_else"
#define K_while "K_while"
#define K_for "K_for"
#define K_in "K_in"
#define K_function "K_function"
#define K_return "K_return"
#define K_call "K_call"

/* Reserved word tokens */
#define R_break "R_break"
#define R_continue "R_continue"

/* Noise word token */
#define NOISE "NOISE"

/* Constant tokens */
#define CONST_DECIMAL "CONST_DECIMAL"
#define CONST_STRING "CONST_STRING"
#define CONST_CHAR "CONST_CHAR"
#define CONST_BOOL "CONST_BOOL"

/* Operator tokens */

/* Arithmetic operators */
#define FLOOR_DIVIDE "FLOOR_DIVIDE"
#define MODULO "MODULO"
#define EXPONENT "EXPONENT"


/* Boolean operators */
#define AND "AND"
#define OR "OR"
#define NOT "NOT"

/* Relational operators */
#define EQUAL "EQUAL"
#define NOT_EQUAL "NOT_EQUAL"
#define LESS_THAN "LESS_THAN"
#define GREATER_THAN "GREATER_THAN"
#define LESS_EQUAL "LESS_EQUAL"
#define GREATER_EQUAL "GREATER_EQUAL"

/* Compound assignment operators */
#define ADD_ASSIGN "ADD_ASSIGN"
#define SUB_ASSIGN "SUB_ASSIGN"
#define MULTIPLY_ASSIGN "MULTIPLY_ASSIGN"
#define DIVIDE_ASSIGN "DIVIDE_ASSIGN"

/*
#define MODULO_ASSIGN "MODULO_ASSIGN"
#define EXPONENT_ASSIGN "EXPONENT_ASSIGN"
*/


/* Delimiter and bracket tokens */
#define LEFT_BRACKET "LEFT_BRACKET"
#define RIGHT_BRACKET "RIGHT_BRACKET"
#define LEFT_BRACE "LEFT_BRACE"
#define RIGHT_BRACE "RIGHT_BRACE"
#define COMMA "COMMA"
#define COLON "COLON"
#define PERIOD "PERIOD"

/* Comment token */
#define COMMENT "COMMENT"

/* Layout tokens */
#define INDENT "INDENT"
#define DEDENT "DEDENT"
#define NEWLINE "NEWLINE"

/* End of file token */
#define END_OF_FILE "EOF"


/* Store a token's line number, name, and lexeme */
void storeToken(int line, char *token, char *lex)
{
    tokenLine = line;

    /* Stop storing if the maximum token limit is reached */
    if (storedTokenCount >= MAX_TOKENS)
        return;

    /* Store the token information */
    tokenLines[storedTokenCount] = line;
    strcpy(tokenNames[storedTokenCount], token);
    strcpy(tokenLexemes[storedTokenCount], lex);

    storedTokenCount++;
}

/* Write the lexical errors and token table to a report file */
void writeTokenTable(FILE *report_fp)
{
    int i;

    fprintf(report_fp, "================================\n");
    fprintf(report_fp, "CLARIFIX LEXICAL ANALYSIS REPORT\n");
    fprintf(report_fp, "================================\n\n");

    /* Write lexical errors */
    fprintf(report_fp, "---------------\n");
    fprintf(report_fp, "LEXICAL ERRORS\n");
    fprintf(report_fp, "---------------\n");

    if (storedErrorCount == 0)
    {
        fprintf(report_fp, "No lexical errors found.\n");
    }
    else
    {
        for (i = 0; i < storedErrorCount; i++)
        {
            fprintf(report_fp, "Line %d: %s\n",
                    errorLines[i], errorMessages[i]);
        }
    }

    /* Write token table */
    fprintf(report_fp, "\nTOKEN TABLE\n");
    fprintf(report_fp, "-----------\n");

    fprintf(report_fp, "+----------+----------------------+--------------------------------------------------------------+\n");
    fprintf(report_fp, "| %-8s | %-20s | %-60s |\n",
            "Line", "Token", "Lexeme");
    fprintf(report_fp, "+----------+----------------------+--------------------------------------------------------------+\n");

    for (i = 0; i < storedTokenCount; i++)
    {

        char displayLexeme[2000];
        int j = 0;
        int k;

        if (strcmp(tokenNames[i], "NEWLINE") == 0)
            strcpy(displayLexeme, "\\n");
        else if (strcmp(tokenNames[i], "EOF") == 0)
            strcpy(displayLexeme, "EOF");

        else
        {

        for (k = 0; tokenLexemes[i][k] != '\0' && j < 1998; k++)
        {
            if (tokenLexemes[i][k] == '\r')
                continue;

            displayLexeme[j++] = tokenLexemes[i][k];
        }
                displayLexeme[j] = '\0';

        }

        int column = 0;
        int p;

        /* Print the first line of the token row */
        fprintf(report_fp, "| %-8d | %-20s | ", tokenLines[i], tokenNames[i]);

        for (p = 0; displayLexeme[p] != '\0'; p++)
        {
            if (displayLexeme[p] == '\n')
            {
                while (column < 60)
                {
                    fputc(' ', report_fp);
                    column++;
                }

                fprintf(report_fp, " |\n");
                fprintf(report_fp, "|          |                      | ");
                column = 0;
            }
            else
            {
                if (column == 60)
                {
                    fprintf(report_fp, " |\n");
                    fprintf(report_fp, "|          |                      | ");
                    column = 0;
                }

                fputc(displayLexeme[p], report_fp);
                column++;
            }
        }

        /* Complete the final line */
        while (column < 60)
        {
            fputc(' ', report_fp);
            column++;
        }

        fprintf(report_fp, " |\n");

            }

    fprintf(report_fp, "+----------+----------------------+--------------------------------------------------------------+\n");

    /* Write total errors */
    fprintf(report_fp, "\nTotal Errors: %d\n", storedErrorCount);
}

/* Store a lexical error and its line number */
void storeError(int line, char *message)
{
    /* Stop storing if the maximum error limit is reached */
    if (storedErrorCount >= MAX_ERRORS)
        return;

    /* Store the error information */
    errorLines[storedErrorCount] = line;
    strcpy(errorMessages[storedErrorCount], message);

    /* Move to the next available storage position */
    storedErrorCount++;
}

/******************************************************/

/*main*/
int main(void)
{
    char filename[100];
    char reportFilename[120];
    FILE *report_fp;

    do
    {
        printf("Enter ClariFix source filename: ");
        scanf("%99s", filename);

        /* Check file extension */
        if (!isClxFile(filename))
        {
            printf("\nError E100: Invalid file extension. Use .clx files only. (ctrl + c to exit program)\n\n");
            continue;
        }

        /* Attempt to open the file */
        in_fp = fopen(filename, "r");

        if (in_fp == NULL)
        {
            printf("ERROR - cannot open file. Please try again.\n\n");
            continue;
        }

        /* Both checks passed */
        break;

    } while (1);

    /* Process the source file */
    /* Initialize lexical analysis */
    indentStack[0] = 0;
    getChar();

    /* Start syntax analysis */
    advance();
    program();

    /* Display syntax analysis results */
    printf("\nSyntax analysis completed.\n");
    printf("Total Syntax Errors: %d\n", syntaxErrorCount);

    if (syntaxErrorCount == 0)
    {
        printf("No syntax errors found.\n");
    }
    else
    {
        printf("Program contains syntax errors.\n");
    }

    /* Remove the .clx extension and create the report filename */
        strcpy(reportFilename, filename);
        reportFilename[strlen(reportFilename) - 4] = '\0';
        strcat(reportFilename, "_tokenTable.txt");

    /* Create the token table report */
    report_fp = fopen(reportFilename, "w");

    if (report_fp != NULL)
    {
        writeTokenTable(report_fp);
        fclose(report_fp);

        printf("\nToken table report saved as: %s\n", reportFilename);
    }
    else
    {
        printf("\nERROR - Could not create token table report.\n");
    }

    fclose(in_fp);

    return 0;
}

/*****************************************************/

int isClxFile(char filename[])
{
    int len = strlen(filename); 

    if (len < 4)  
        return 0;   

    //points to the start of the file extentions [ex. file(.txt)] then compare if its .clx
    return strcmp(filename + len - 4, ".clx") == 0;
}


/*****************************************************/

/* lookup - a function to lookup operators and parentheses
   and return the token */
char *lookup(char ch)
{
    switch (ch)
    {
        case '(':
            addChar();
            getChar();
            nextToken = LEFT_PAREN;
            break;

        case ')':
            addChar();
            getChar();
            nextToken = RIGHT_PAREN;
            break;

        case '[':
            addChar();
            getChar();
            nextToken = LEFT_BRACKET;
            break;

        case ']':
            addChar();
            getChar();
            nextToken = RIGHT_BRACKET;
            break;

        case '{':
            addChar();
            getChar();
            nextToken = LEFT_BRACE;
            break;

        case '}':
            addChar();
            getChar();
            nextToken = RIGHT_BRACE;
            break;

        case '+':
            addChar();
            getChar();

            if (nextChar == '=')
            {
                addChar();
                nextToken = ADD_ASSIGN;
                getChar();
            }
            else
            {
                nextToken = ADD;
            }
            break;

        case '-':
            addChar();
            getChar();

            if (nextChar == '=')
            {
                addChar();
                nextToken = SUB_ASSIGN;
                getChar();
            }
            else
            {
                nextToken = SUBTRACT;
            }
            break;

        case '*':
            addChar();
            getChar();

            if (nextChar == '=')
            {
                addChar();
                nextToken = MULTIPLY_ASSIGN;
                getChar();
            }
            else
            {
                nextToken = MULTIPLY;
            }
            break;

        case '/':
            addChar();
            getChar();
            nextToken = DIVIDE;
            break;

        case '%':
            addChar();
            getChar();
            nextToken = MODULO;
            break;

        case '^':
            addChar();
            getChar();
            nextToken = EXPONENT;
            break;

        case '!':
            addChar();
            getChar();
            nextToken = NOT;
            break;

        case '=':
            addChar();
            getChar();
            nextToken = ASSIGN;
            break;

        case '<':
            addChar();
            getChar();
            nextToken = LESS_THAN;
            break;

        case '>':
            addChar();
            getChar();
            nextToken = GREATER_THAN;
            break;

        case '.':
            addChar();
            getChar();
            nextToken = PERIOD;
            break;

        case ',':
            addChar();
            getChar();
            nextToken = COMMA;
            break;

        case ':':
            addChar();
            getChar();
            nextToken = COLON;
            break;

        default:
            addChar();
            getChar();
            nextToken = ERROR_TOKEN;
            break;
    }

    return nextToken;
}


/*****************************************************/

/* addChar - a function to add nextChar to lexeme[] for printing */
void addChar()
{
    if (lexLen < 99)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = '\0';
    }

    else if (!lexemeOverflow)
    {
        printf("Error - lexeme is too long\n");

        /* Store the error and its original line number */
        storeError(lineNumber, "Error - lexeme is too long");

        lexemeOverflow = 1;
        errorCount++;
    }
}

/*   handling multi line comment for printing */
void addCommentChar()
{
    if (commentLen < 999)
    {
        commentLexeme[commentLen++] = nextChar;
        commentLexeme[commentLen] = '\0';
    }
}

/*****************************************************/

/* getChar - a function to get the next character of
   input and determine its character class */

void getChar()
{
    if ((nextChar = getc(in_fp)) != EOF)
    {
        if (isalpha(nextChar) || nextChar == '_')
            charClass = LETTER;

        else if (isdigit(nextChar))
            charClass = DIGIT;

        else if (nextChar == '\n' || nextChar == '\r')
            charClass = NEW_LINE;

        else if (strchr("+-*/%^=&|!<>(){}[],:.\"'#", nextChar))
            charClass = SPECIAL;
        else
            charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}
/*****************************************************/

/* getNonBlank - a function to call getChar until it
   returns a non-whitespace character */
void getNonBlank()
{
    while (nextChar == ' ' || nextChar == '\t')
    {
        if (atLineStart)
        {
            if (nextChar == ' ')
                currentIndent++;
            else
                currentIndent += 4 - (currentIndent % 4);
        }

        getChar();
    }

    if (atLineStart && !indentationChecked)
    {
        indentationChecked = 1;
        indentationError = 0;

        if (currentIndent % 4 != 0)
        {
            indentationError = 1;

            printf("ERROR E108: Bad indentation at line %d.\n", lineNumber);

            /* Store the error and its original line number */
            storeError(lineNumber, "ERROR E108: Bad indentation");

            errorCount++;
        }
    }

}

/*****************************************************/
char *checkKeyword()
{
    if (strcmp(lexeme, "set") == 0)
        return K_set;

    else if (strcmp(lexeme, "show") == 0)
        return K_show;

    else if (strcmp(lexeme, "ask") == 0)
        return K_ask;

    else if (strcmp(lexeme, "if") == 0)
        return K_if;

    else if (strcmp(lexeme, "elif") == 0)
        return K_elif;

    else if (strcmp(lexeme, "else") == 0)
        return K_else;

    else if (strcmp(lexeme, "while") == 0)
        return K_while;

    else if (strcmp(lexeme, "for") == 0)
        return K_for;

    else if (strcmp(lexeme, "in") == 0)
        return K_in;

    else if (strcmp(lexeme, "function") == 0)
        return K_function;

    else if (strcmp(lexeme, "return") == 0)
        return K_return;

    else if (strcmp(lexeme, "call") == 0)
    return K_call;

    else if (strcmp(lexeme, "break") == 0)
        return R_break;

    else if (strcmp(lexeme, "continue") == 0)
        return R_continue;

    else if (strcmp(lexeme, "true") == 0 ||
            strcmp(lexeme, "false") == 0)
        return CONST_BOOL;

    else if (strcmp(lexeme, "to") == 0 ||
         strcmp(lexeme, "then") == 0 ||
         strcmp(lexeme, "the") == 0 ||
         strcmp(lexeme, "of") == 0)
        return NOISE;

    else
        return IDENTIFIER;
}



/*****************************************************/

/* lexer */
char *lex()
{
    int commentStartLine = 0;
    lexLen = 0;
    lexemeOverflow = 0;
    nextToken = "";
    getNonBlank();

    if (indentationError)
    {
        atLineStart = 0;
        nextToken = ERROR_TOKEN;

        storeToken(lineNumber, nextToken, "");

        printf("Next token is: %s, Next lexeme is %s\n", nextToken, lexeme);

        indentationError = 0;
        return nextToken;
    }

    if (charClass == NEW_LINE)
    {
        addChar();

        if (nextChar == '\r')
        {
            getChar();

            if (nextChar == '\n')
                addChar();
        }

        else if (nextChar == '\n')
        {
            
        }

        getChar();

        lineNumber++;
        currentIndent = 0;
        indentationChecked = 0;

        atLineStart = 1;

        if (!hasStatement)
        {
            lexeme[0] = '\0';
            hasStatement = 0;
            return lex();
        }
    
        nextToken = NEWLINE;
        hasStatement = 0;

        /* Store a readable newline representation in the token table */
        storeToken(lineNumber - 1, nextToken, "\\n");

        printf("Next token is: %s, Next lexeme is %s\n", nextToken, lexeme);
        return nextToken;

    }


    //start if INDENTATION handling
    if (charClass != EOF)
    {
        if (atLineStart && currentIndent % 4 == 0 &&
            currentIndent > indentStack[indentTop])
            {
            if (indentTop >= 99)
                {
                printf("ERROR E108: Maximum indentation depth exceeded at line %d.\n", lineNumber);

                /* Store the error and its original line number */
                storeError(lineNumber, "ERROR E108: Maximum indentation depth exceeded");

                errorCount++;
                                }
            else
                {
                    indentTop++;
                    indentStack[indentTop] = currentIndent;
                    nextToken = INDENT;

                  
                    storeToken(lineNumber, nextToken, lexeme);    /* Store the indentation token */

                    printf("Next token is: %s, Next lexeme is %s\n", nextToken, lexeme);
                    return nextToken;
            }
            }

            if (atLineStart && currentIndent % 4 == 0 &&
                currentIndent < indentStack[indentTop])
                {
                    int i;
                    int found = 0;

                    for (i = 0; i <= indentTop; i++)
                    {
                        if (indentStack[i] == currentIndent)
                        {
                            found = 1;
                            break;
                        }
                    }

                    if (found)
                    {
                        indentTop--;
                        lexeme[0] = '\0';
                        nextToken = DEDENT;

                    
                        storeToken(lineNumber, nextToken, lexeme);    /* Store the dedentation token */

                        printf("Next token is: %s, Next lexeme is %s\n", nextToken, lexeme);
                        return nextToken;
                    }

                    else
                    {
                        printf("ERROR E108: Bad indentation at line %d.\n", lineNumber);

                        /* Store the error and its original line number */
                        storeError(lineNumber, "ERROR E108: Bad indentation");

                        errorCount++;

                        atLineStart = 0;
                        nextToken = ERROR_TOKEN;

                        /* Store the error token */
                        storeToken(lineNumber, nextToken, lexeme);

                        printf("Next token is: %s, Next lexeme is %s\n", nextToken, lexeme);

                        return nextToken;
                    }
                }
    } /* Closes if (charClass != EOF) */

    atLineStart = 0;

    switch (charClass)
    {
        /* Parse identifiers */
        case LETTER:
            addChar();
            getChar();

            while (charClass == LETTER || charClass == DIGIT)
            {
                addChar();
                getChar();
            }

            if (lexLen > 32)
            {
                printf("Error E104: Identifier exceeds 32 characters: %s\n", lexeme);

                /* Store the error and its original line number */
                char errorMessage[200];
                sprintf(errorMessage, "Error E104: Identifier exceeds 32 characters: %s", lexeme);
                storeError(lineNumber, errorMessage);

                nextToken = ERROR_TOKEN;
            }

            else
            {
                nextToken = checkKeyword();

                if (strcmp(nextToken, NOISE) != 0)
                    hasStatement = 1;
            }

            break;

        /* Parse integer literals */
        case DIGIT:
            addChar();
            getChar();

            while (charClass == DIGIT)
            {
                addChar();
                getChar();
            }

            if (charClass == LETTER)
            {
                while (charClass == LETTER || charClass == DIGIT)
                {
                    addChar();
                    getChar();
                }

                printf("Error E110: Name cannot start with a digit: %s\n", lexeme);

                /* Store the error and its original line number */
                char errorMessage[200];
                sprintf(errorMessage, "Error E110: Name cannot start with a digit: %s", lexeme);
                storeError(lineNumber, errorMessage);

                nextToken = ERROR_TOKEN;
            }
                        
            else if (charClass == SPECIAL && nextChar == '.')
            {
                addChar();
                getChar();

                if (charClass == DIGIT)
                {
                    while (charClass == DIGIT)
                    {
                        addChar();
                        getChar();
                    }

                    if (charClass == LETTER)
                    {
                    while (charClass == LETTER || charClass == DIGIT)
                    {
                        addChar();
                        getChar();
                    }

                    printf("Error E111: Invalid decimal: %s\n", lexeme);

                    /* Store the error and its original line number */
                    char errorMessage[200];
                    sprintf(errorMessage, "Error E111: Invalid decimal: %s", lexeme);
                    storeError(lineNumber, errorMessage);

                    nextToken = ERROR_TOKEN;
                    }
                    else
                    {
                        nextToken = CONST_DECIMAL;
                    }
                }

                else
                {
                if (charClass != EOF)
                    ungetc(nextChar, in_fp);

                ungetc('.', in_fp);
                getChar();

                lexLen--;
                lexeme[lexLen] = '\0';
                nextToken = CONST_INT;
                }
            }
            else
            {
                nextToken = CONST_INT;
            }

            break;

        /* Special symbols */
        case SPECIAL:
                if (nextChar == '"')
                    {
                        addChar();
                        getChar();

                        while (charClass != EOF &&
                                nextChar != '"' &&
                                nextChar != '\n' &&
                                nextChar != '\r')
                        {
                            if (nextChar == '\\')
                                {
                                    addChar();
                                    getChar();

                                    if (nextChar == '\n' || nextChar == '\r' || charClass == EOF)
                                        break;

                                    if (nextChar != 'n' &&
                                        nextChar != '"' &&
                                        nextChar != '\'' &&
                                        nextChar != '\\')
                                    {
                                        printf("Error E112: Unknown escape sequence\n");

                                        /* Store the error and its original line number */
                                        storeError(lineNumber, "Error E112: Unknown escape sequence");

                                        nextToken = ERROR_TOKEN;
                                    }

                                    addChar();
                                    getChar();
                                }
                                else
                                {
                                    addChar();
                                    getChar();
                                }
                        }

                        if (nextChar == '"')
                        {
                            addChar();
                            getChar();
                            if (strcmp(nextToken, ERROR_TOKEN) != 0)
                                nextToken = CONST_STRING;
                        }

                        else
                        {
                            printf("Error E102: Unclosed string literal\n");

                            /* Store the error and its original line number */
                            storeError(lineNumber, "Error E102: Unclosed string literal");

                            nextToken = ERROR_TOKEN;
                        }
                    }

                else if (nextChar == '\'')
                    {
                        addChar();
                        getChar();

                        if (nextChar == '\'')
                        {
                            printf("Error E113: Empty character literal\n");

                            /* Store the error and its original line number */
                            storeError(lineNumber, "Error E113: Empty character literal");

                            nextToken = ERROR_TOKEN;

                            addChar();
                            getChar();
                        }
                        else
                        {
                            if (nextChar == '\\')
                            {
                                addChar();
                                getChar();
                                if (nextChar != 'n' &&
                                    nextChar != '"' &&
                                    nextChar != '\'' &&
                                    nextChar != '\\')
                                {

                                printf("Error E112: Unknown escape sequence\n");

                                /* Store the error and its original line number */
                                storeError(lineNumber, "Error E112: Unknown escape sequence");

                                nextToken = ERROR_TOKEN;

                                }

                                if (nextChar != '\n' &&
                                    nextChar != '\r' &&
                                    charClass != EOF)
                                {
                                    addChar();
                                    getChar();
                                }
                            }

                            else
                            {
                                addChar();
                                getChar();
                            }

                            if (nextChar == '\'')
                            {
                                addChar();
                                getChar();

                                if (strcmp(nextToken, ERROR_TOKEN) != 0)
                                    nextToken = CONST_CHAR;
                            }

                                else if (charClass == EOF ||
                                        nextChar == '\n' ||
                                        nextChar == '\r' ||
                                        nextChar == '.')
                                {
                                    printf("Error E102: Unclosed character literal\n");

                                    /* Store the error and its original line number */
                                    storeError(lineNumber, "Error E102: Unclosed character literal");

                                    nextToken = ERROR_TOKEN;
                                }
                                    else
                                    {
                                        printf("Error E113: Invalid character literal\n");

                                        /* Store the error and its original line number */
                                        storeError(lineNumber, "Error E113: Invalid character literal");
                                        
                                        nextToken = ERROR_TOKEN;

                                        while (charClass != EOF &&
                                            nextChar != '\'' &&
                                            nextChar != '\n' &&
                                            nextChar != '\r')
                                        {
                                            addChar();
                                            getChar();
                                        }

                                        if (nextChar == '\'')
                                        {
                                            addChar();
                                            getChar();
                                        }
                                    }
                        }
                    }

                else if (nextChar == '#')
                {
                    addChar();
                    getChar();

                    while (charClass != EOF &&
                        nextChar != '\n' &&
                        nextChar != '\r')
                    {
                        addChar();
                        getChar();
                    }

                    nextToken = COMMENT;

                }

                else if (nextChar == '.')
                    {
                        addChar();
                        getChar();

                        if (charClass == DIGIT)
                        {
                            while (charClass == DIGIT)
                            {
                                addChar();
                                getChar();
                            }

                            nextToken = CONST_DECIMAL;
                        }
                        else
                        {
                            lexeme[0] = '.';
                            lexeme[1] = '\0';
                            nextToken = PERIOD;
                        }
                    }

                else if (nextChar == '/')
                {
                    addChar();
                    getChar();

                    if (nextChar == '/')
                    {
                        addChar();
                        getChar();
                        nextToken = FLOOR_DIVIDE;
                    }

                    else if (nextChar == '=')
                    {
                        addChar();
                        getChar();
                        nextToken = DIVIDE_ASSIGN;
                    }

                    else if (nextChar == '*')
                    {
                        commentStartLine = lineNumber;
                        
                        /* Reset the separate comment buffer */
                        commentLen = 0;
                        commentLexeme[0] = '\0';

                        /* Store the opening slash */
                        commentLexeme[commentLen++] = '/';
                        commentLexeme[commentLen] = '\0';

                        addCommentChar();

                        if (nextChar == '\n')
                            lineNumber++;

                        getChar();

                        nextToken = ERROR_TOKEN;

                            while (charClass != EOF)
                            {
                                addCommentChar();

                                if (nextChar == '\n')
                                    lineNumber++;

                                getChar();

                            if (commentLen >= 2 &&
                                commentLexeme[commentLen - 1] == '*' &&
                                nextChar == '/')
                            {
                                addCommentChar();
                                getChar();
                                nextToken = COMMENT;
                                break;
                            }
                        }

                        if (strcmp(nextToken, ERROR_TOKEN) == 0)
                        {
                            printf("Error E106: Unclosed block comment\n");

                            /* Store the error using the comment's starting line */
                            storeError(commentStartLine, "Error E106: Unclosed block comment");

                            errorCount++;
                        }
                    }

                    else
                    {
                        nextToken = DIVIDE;
                    }
                }

                else if (nextChar == '&')
                {
                    addChar();
                    getChar();

                    if (nextChar == '&')
                    {
                        addChar();
                        getChar();
                        nextToken = AND;
                    }

                    else
                    {
                    printf("Error E103: Unsupported character: &\n");

                    /* Store the error and its original line number */
                    storeError(lineNumber, "Error E103: Unsupported character: &");

                    nextToken = ERROR_TOKEN;
                    }
                }

                else if (nextChar == '|')
                {
                    addChar();
                    getChar();

                    if (nextChar == '|')
                    {
                        addChar();
                        getChar();
                        nextToken = OR;
                    }
                    else
                    {
                    printf("Error E103: Unsupported character: |\n");

                    /* Store the error and its original line number */
                    storeError(lineNumber, "Error E103: Unsupported character: |");

                    nextToken = ERROR_TOKEN;
                    }
                }

                else if (nextChar == '!')
                {
                    addChar();
                    getChar();

                    if (nextChar == '=')
                    {
                        addChar();
                        getChar();
                        nextToken = NOT_EQUAL;
                    }
                    else
                    {
                        nextToken = NOT;
                    }
                }

                else if (nextChar == '=')
                {
                    addChar();
                    getChar();

                    if (nextChar == '=')
                    {
                        addChar();
                        getChar();
                        nextToken = EQUAL;
                    }
                    else
                    {
                        nextToken = ASSIGN;
                    }
                }

                else if (nextChar == '<')
                {
                    addChar();
                    getChar();

                    if (nextChar == '=')
                    {
                        addChar();
                        getChar();
                        nextToken = LESS_EQUAL;
                    }
                    else
                    {
                        nextToken = LESS_THAN;
                    }
                }

                else if (nextChar == '>')
                {
                    addChar();
                    getChar();

                    if (nextChar == '=')
                    {
                        addChar();
                        getChar();
                        nextToken = GREATER_EQUAL;
                    }
                    else
                    {
                        nextToken = GREATER_THAN;
                    }
                }

                else
                {
                    lookup(nextChar);
                }

            break;

        /* Handle unsupported characters */
        case UNKNOWN:
            if ((unsigned char)nextChar == 0xE2)
            {
                int secondByte = getc(in_fp);
                int thirdByte = getc(in_fp);

                if (secondByte == 0x80 &&
                    (thirdByte == 0x98 || thirdByte == 0x99 ||
                    thirdByte == 0x9C || thirdByte == 0x9D))
                {
                    printf("Error E109: Curly quotes are not allowed.\n");
                    storeError(lineNumber, "Error E109: Curly quotes are not allowed.");
                    lexLen = 0;
                    lexeme[0] = '\0';
                    nextToken = ERROR_TOKEN;
                    getChar();
                }
                else
                {
                printf("Error E103: Unsupported character.\n");

                /* Store the error and its original line number */
                storeError(lineNumber, "Error E103: Unsupported character.");

                nextToken = ERROR_TOKEN;
                getChar();
                }
            }
            else
            {
                printf("Error E103: Unsupported character: %c\n", nextChar);

                /* Store the error and its original line number */
                char errorMessage[200];
                sprintf(errorMessage, "Error E103: Unsupported character: %c", nextChar);
                storeError(lineNumber, errorMessage);

                addChar();
                nextToken = ERROR_TOKEN;
                getChar();
            }
            break;

        /* EOF */
        case EOF:
            if (indentTop > 0)
            {
                indentTop--;
                lexeme[0] = '\0';
                nextToken = DEDENT;
                break;
            }

            nextToken = END_OF_FILE;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;

           
            break;

    } /* End of switch */

if (lexemeOverflow)
    nextToken = ERROR_TOKEN;

if (strcmp(nextToken, COMMENT) != 0 &&
    strcmp(nextToken, NOISE) != 0 &&
    strcmp(nextToken, ERROR_TOKEN) != 0)
{
    hasStatement = 1;
}

/* Store the recognized token */
if ((strcmp(nextToken, COMMENT) == 0 ||
     strcmp(nextToken, ERROR_TOKEN) == 0) && commentLen > 0)
{
    storeToken(commentStartLine, nextToken, commentLexeme);

    printf("Next token is: %s, Next lexeme is %s\n",
           nextToken, commentLexeme);

    /* Reset the block comment buffer after storing */
    commentLen = 0;
    commentLexeme[0] = '\0';
}
else
{
    storeToken(lineNumber, nextToken, lexeme);

    printf("Next token is: %s, Next lexeme is %s\n",
           nextToken, lexeme);
}

return nextToken;
} /* End of function lex */

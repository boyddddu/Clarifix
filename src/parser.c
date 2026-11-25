#include <stdio.h>
#include <string.h>
#include "parser.h"


/* Access existing lexer variables and functions */
extern char *nextToken;
extern char lexeme[100];
extern int lineNumber;
extern char *lex(void);
extern int tokenLine;



int parseDepth = 0; //pang indentation sa output
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Retrieve the next token, ignoring comments and noise words */
void advance(void)
{
    do
    {
        lex();
    }
    while (strcmp(nextToken, "COMMENT") == 0 ||
           strcmp(nextToken, "NOISE") == 0);
}

/* Check if the current token matches the expected token */
int check(const char *expected)
{
    return strcmp(nextToken, expected) == 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Consume the current token if it matches */
int match(const char *expected)
{
    if (check(expected))
    {
        advance();
        return 1;
    }

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int syntaxErrorCount = 0;
int panicMode = 0;

const char *getSyntaxErrorCode(const char *expected)
{
    if (expected == NULL)
    {
        if (check("INDENT") || check("DEDENT"))
            return "E207";

        return "E200";
    }

    if (strcmp(expected, "IDENTIFIER") == 0)
        return "E201";

    if (strcmp(expected, "an expression") == 0 ||
        strcmp(expected, "a literal") == 0 ||
        strcmp(expected, "a number") == 0)
        return "E202";

    if (strcmp(expected, "PERIOD") == 0)
        return "E203";

    if (strcmp(expected, "COLON") == 0)
        return "E204";

    if (strcmp(expected, "NEWLINE") == 0)
        return "E205";

    if (strcmp(expected, "INDENT") == 0)
        return "E206";

    if (strcmp(expected, "DEDENT") == 0)
        return "E208";

    if (strcmp(expected, "an assignment operator") == 0)
        return "E209";

    if (strcmp(expected, "RIGHT_PAREN") == 0)
        return "E210";

    if (strcmp(expected, "RIGHT_BRACKET") == 0)
        return "E211";

    if (strcmp(expected, "RIGHT_BRACE") == 0)
        return "E212";

    if (strcmp(expected, "LEFT_PAREN") == 0 ||
        strcmp(expected, "LEFT_BRACKET") == 0 ||
        strcmp(expected, "LEFT_BRACE") == 0)
        return "E213";

    if (strcmp(expected, "COMMA") == 0)
        return "E214";

    if (strcmp(expected, "CONST_INT") == 0)
        return "E215";

    if (strcmp(expected, "an iterable") == 0)
        return "E216";

    if (strncmp(expected, "K_", 2) == 0 ||
        strncmp(expected, "R_", 2) == 0)
        return "E217";

    return "E200";
}

/* Report a syntax error and activate panic mode */
void syntax_error(const char *expected)
{
    if (panicMode)
        return;

    const char *code = getSyntaxErrorCode(expected);
    char message[200];

    if (expected != NULL)
    {
        snprintf(message, sizeof(message),
                 "Expected %s but found %s",
                 expected, nextToken);
    }
    else
    {
        snprintf(message, sizeof(message),
                 "Unexpected token %s", nextToken);
    }

    /* Print the syntax error */
    printf("Error %s at line %d: %s\n",
           code, tokenLine, message);

    syntaxErrorCount++;
    panicMode = 1;
}

/* Check and consume a required token */
int expect(const char *expected)
{
    if (match(expected))
        return 1;

    syntax_error(expected);

    return 0;
}

/* Skip an invalid indented block */
/* Skip an invalid indented block */
void skip_block(void)
{
    int recoveryDepth = 0;

    printf("Panic Mode: Skipping malformed block.\n");

    do
    {
        if (check("INDENT"))
            recoveryDepth++;

        else if (check("DEDENT"))
            recoveryDepth--;

        advance();

    } while (recoveryDepth > 0 && !check("EOF"));
}

/* Panic-mode error recovery */
void synchronize(void)
{
    printf("\nPanic Mode: Recovering...\n");

    while (!check("PERIOD") &&
           !check("NEWLINE") &&
           !check("INDENT") &&
           !check("DEDENT") &&
           !check("EOF"))
    {
        advance();
    }

    if (match("PERIOD"))
        match("NEWLINE");

    else
        match("NEWLINE");

    /* Skip an associated malformed block */
    if (check("INDENT"))
        skip_block();

    panicMode = 0;

    printf("Panic Mode: Recovery completed.\n\n");
}

/* Skip a conditional header and its associated block */
void skip_clause(void)
{
    while (!check("NEWLINE") &&
           !check("INDENT") &&
           !check("DEDENT") &&
           !check("EOF"))
    {
        advance();
    }

    match("NEWLINE");

    if (check("INDENT"))
        skip_block();
}

/* Skip the entire malformed conditional chain */
void skip_conditional_chain(void)
{
    printf("\nPanic Mode: Recovering...\n");
    printf("Panic Mode: Skipping malformed conditional chain.\n");

    skip_clause();

    while (match("K_elif"))
    {
        skip_clause();
    }

    if (match("K_else"))
    {
        skip_clause();
    }

    panicMode = 0;

    printf("Panic Mode: Recovery completed.\n\n");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Enter a grammar rule */
void enter(const char *rule)
{
    for (int i = 0; i < parseDepth; i++)
        printf(" ");

    printf("Enter <%s>\n", rule);
    parseDepth++;
}

void leave(const char *rule)
{
    parseDepth--;

    for (int i = 0; i < parseDepth; i++)
        printf(" ");

    printf("Exit <%s>\n", rule);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Forward declarations */
void statement(void);
void simple_stmt(void);
void compound_stmt(void);
void set_stmt(void);
void show_stmt(void);
void ask_stmt(void);
void return_stmt(void);
void break_stmt(void);
void continue_stmt(void);
void assign_stmt(void);
void call_stmt(void);
void if_stmt(void);
void for_stmt(void);
void while_stmt(void);
void function_stmt(void);
void expr(void);
void index_expr(void);
void assign_op(void);
void call_args(void);
void iterable(void);
void range(void);
void range_int(void);
void param_list(void);
void block(void);
void or_expr(void);
void and_expr(void);
void equality(void);
void relational(void);
void additive(void);
void term(void);
void negate(void);
void power(void);
void primary(void);
void arg_list(void);
void list(void);
void literal(void);
void number(void);

void skip_block(void);
void synchronize(void);
void skip_clause(void);
void skip_conditional_chain(void);

/* <program> -> { <statement> } EOF */
void program(void)
{
    enter("program");

    while (!check("EOF"))
    {
        statement();

        if (panicMode)
        {
            synchronize();

            /* Unexpected DEDENT at program level */
            while (check("DEDENT"))
                advance();
        }
    }

    leave("program");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <statement> -> <simple-stmt> "." [ NEWLINE ]
                | <compound-stmt> */
void statement(void)
{
    enter("statement");

    if (check("K_set") ||
        check("K_show") ||
        check("K_ask") ||
        check("K_return") ||
        check("R_break") ||
        check("R_continue") ||
        check("IDENTIFIER") ||
        check("K_call"))
    {
        simple_stmt();

        if (!panicMode)
        {
            if (expect("PERIOD"))
                match("NEWLINE");
        }
    }
    else if (check("K_if") ||
             check("K_while") ||
             check("K_for") ||
             check("K_function"))
    {
        compound_stmt();
    }
    else
    {
        syntax_error(NULL);
    }

    leave("statement");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void simple_stmt(void)
{
    enter("simple-stmt");

    if (check("K_set"))
        set_stmt();

    else if (check("K_show"))
        show_stmt();

    else if (check("K_ask"))
        ask_stmt();

    else if (check("K_return"))
        return_stmt();

    else if (check("R_break"))
        break_stmt();

    else if (check("R_continue"))
        continue_stmt();

    else if (check("IDENTIFIER"))
        assign_stmt();

    else if (check("K_call"))
        call_stmt();

    else
    {
        syntax_error(NULL);
    }

    leave("simple-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void compound_stmt(void)
{
    enter("compound-stmt");

    if (check("K_if"))
        if_stmt();

    else if (check("K_for"))
        for_stmt();

    else if (check("K_while"))
        while_stmt();

    else if (check("K_function"))
        function_stmt();

    else
    {
        syntax_error(NULL);
    }

    leave("compound-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <set-stmt> -> "set" IDENTIFIER [ "=" <expr> ] */
void set_stmt(void)
{
    enter("set-stmt");

    if (expect("K_set") && expect("IDENTIFIER"))
    {
        if (match("ASSIGN"))
        {
            expr();
        }
    }

    leave("set-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <assign-stmt> -> IDENTIFIER { <index> } <assign-op> <expr> */
void assign_stmt(void)
{
    enter("assign-stmt");

    if (expect("IDENTIFIER"))
    {
        while (check("LEFT_BRACKET") && !panicMode)
        {
            index_expr();
        }

        if (!panicMode)
            assign_op();

        if (!panicMode)
            expr();
    }

    leave("assign-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* <assign-op> -> "=" | "+=" | "-=" | "*=" | "/=" */
void assign_op(void)
{
    enter("assign-op");

    if (check("ASSIGN") || check("ADD_ASSIGN") || check("SUB_ASSIGN") ||
        check("MULTIPLY_ASSIGN") || check("DIVIDE_ASSIGN"))
    {
        advance();
    }
    else
    {
        syntax_error("an assignment operator");
    }

    leave("assign-op");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <show-stmt> -> "show" <expr> */
void show_stmt(void)
{
    enter("show-stmt");

    if (expect("K_show"))
        expr();

    leave("show-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <ask-stmt> -> "ask" IDENTIFIER */
void ask_stmt(void)
{
    enter("ask-stmt");

    if (expect("K_ask"))
        expect("IDENTIFIER");

    leave("ask-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <return-stmt> -> "return" [ <expr> ] */
void return_stmt(void)
{
    enter("return-stmt");

    if (expect("K_return") && !check("PERIOD"))
        expr();

    leave("return-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <break-stmt> -> "break" */
void break_stmt(void)
{
    enter("break-stmt");
    expect("R_break");
    leave("break-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <continue-stmt> -> "continue" */
void continue_stmt(void)
{
    enter("continue-stmt");
    expect("R_continue");
    leave("continue-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <call-stmt> -> "call" IDENTIFIER <call-args> */
void call_stmt(void)
{
    enter("call-stmt");

    if (expect("K_call") && expect("IDENTIFIER"))
        call_args();

    leave("call-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <call-args> -> "(" [ <arg-list> ] ")" */
void call_args(void)
{
    enter("call-args");

    if (expect("LEFT_PAREN"))
    {
        if (!check("RIGHT_PAREN"))
            arg_list();

        if (!panicMode)
            expect("RIGHT_PAREN");
    }

    leave("call-args");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <while-stmt> -> "while" <expr> ":" <block> */
void while_stmt(void)
{
    enter("while-stmt");

    if (expect("K_while"))
    {
        expr();

        if (!panicMode && expect("COLON"))
            block();
    }

    leave("while-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <if-stmt> -> "if" <expr> ":" <block>
                { "elif" <expr> ":" <block> }
                [ "else" ":" <block> ] */
void if_stmt(void)
{
    enter("if-stmt");

    /* Initial if clause */
    if (expect("K_if"))
    {
        expr();

        if (!panicMode)
            expect("COLON");

        /* Invalid initial header: skip entire chain */
        if (panicMode)
        {
            skip_conditional_chain();

            leave("if-stmt");
            return;
        }

        block();

        if (panicMode)
            synchronize();

        /* Handle multiple elif clauses */
        while (match("K_elif"))
        {
            expr();

            if (!panicMode && expect("COLON"))
                block();

            if (panicMode)
            {
                printf("\nPanic Mode: Recovering...\n");

                skip_clause();
                panicMode = 0;

                printf("Panic Mode: Recovery completed.\n\n");
            }
        }

        /* Optional else clause */
        if (match("K_else"))
        {
            if (expect("COLON"))
                block();

            if (panicMode)
            {
                printf("\nPanic Mode: Recovering...\n");

                skip_clause();
                panicMode = 0;

                printf("Panic Mode: Recovery completed.\n\n");
            }
        }
    }

    leave("if-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <for-stmt> -> "for" IDENTIFIER "in" <iterable> ":" <block> */
void for_stmt(void)
{
    enter("for-stmt");

    if (expect("K_for") && expect("IDENTIFIER") && expect("K_in"))
    {
        iterable();

        if (!panicMode && expect("COLON"))
            block();
    }

    leave("for-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <iterable> -> <range> | <list> | IDENTIFIER { <index> } */
void iterable(void)
{
    enter("iterable");

    if (check("LEFT_BRACKET"))
        range();
    else if (check("LEFT_BRACE"))
        list();
    else if (match("IDENTIFIER"))
    {
        while (!panicMode && check("LEFT_BRACKET"))
            index_expr();
    }
    else
    {
        syntax_error("an iterable");
    }

    leave("iterable");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <range> -> "[" <range-int> "," <range-int> [ "," <range-int> ] "]" */
void range(void)
{
    enter("range");

    if (expect("LEFT_BRACKET"))
    {
        range_int();

        if (!panicMode && expect("COMMA"))
            range_int();

        if (!panicMode && match("COMMA"))
            range_int();

        if (!panicMode)
            expect("RIGHT_BRACKET");
    }

    leave("range");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <range-int> -> [ "-" ] CONST_INT */
void range_int(void)
{
    enter("range-int");

    match("SUBTRACT");
    expect("CONST_INT");

    leave("range-int");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <function-stmt> -> "function" IDENTIFIER "(" [ <param-list> ] ")" ":" <block> */
void function_stmt(void)
{
    enter("function-stmt");

    if (expect("K_function") && expect("IDENTIFIER") && expect("LEFT_PAREN"))
    {
        if (!check("RIGHT_PAREN"))
            param_list();

        if (!panicMode && expect("RIGHT_PAREN") && expect("COLON"))
            block();
    }

    leave("function-stmt");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <param-list> -> IDENTIFIER { "," IDENTIFIER } */
void param_list(void)
{
    enter("param-list");

    expect("IDENTIFIER");
    while (!panicMode && match("COMMA"))
        expect("IDENTIFIER");

    leave("param-list");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <block> -> NEWLINE INDENT <statement> { <statement> } DEDENT */
/* <block> -> NEWLINE INDENT <statement> { <statement> } DEDENT */
void block(void)
{
    enter("block");

    if (!expect("NEWLINE"))
    {
        leave("block");
        return;
    }

    if (!expect("INDENT"))
    {
        /* Preserve the next token for the enclosing scope */
        panicMode = 0;

        leave("block");
        return;
    }

    /* Require at least one statement */
    statement();

    if (panicMode)
        synchronize();

    /* Parse the remaining statements */
    while (!check("DEDENT") && !check("EOF"))
    {
        statement();

        if (panicMode)
            synchronize();
    }

    expect("DEDENT");

    leave("block");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <expr> -> <or-expr> */
void expr(void)
{
    enter("expr");
    or_expr();
    leave("expr");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <or-expr> -> <and-expr> { "||" <and-expr> } */
void or_expr(void)
{
    enter("or-expr");

    and_expr();
    while (!panicMode && match("OR"))
        and_expr();

    leave("or-expr");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <and-expr> -> <equality> { "&&" <equality> } */
void and_expr(void)
{
    enter("and-expr");

    equality();
    while (!panicMode && match("AND"))
        equality();

    leave("and-expr");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <equality> -> <relational> [ ( "==" | "!=" ) <relational> ] */
void equality(void)
{
    enter("equality");

    relational();
    if (!panicMode && (match("EQUAL") || match("NOT_EQUAL")))
        relational();

    leave("equality");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <relational> -> <additive> [ ( "<" | ">" | "<=" | ">=" ) <additive> ] */
void relational(void)
{
    enter("relational");

    additive();
    if (!panicMode &&
        (match("LESS_THAN") || match("GREATER_THAN") ||
         match("LESS_EQUAL") || match("GREATER_EQUAL")))
    {
        additive();
    }

    leave("relational");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <additive> -> <term> { ( "+" | "-" ) <term> } */
void additive(void)
{
    enter("additive");

    term();
    while (!panicMode && (match("ADD") || match("SUBTRACT")))
        term();

    leave("additive");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <term> -> <negate> { ( "*" | "/" | "%" | "//" ) <negate> } */
void term(void)
{
    enter("term");

    negate();
    while (!panicMode &&
           (match("MULTIPLY") || match("DIVIDE") ||
            match("MODULO") || match("FLOOR_DIVIDE")))
    {
        negate();
    }

    leave("term");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <negate> -> ( "-" | "!" ) <negate> | <power> */
void negate(void)
{
    enter("negate");

    if (match("SUBTRACT") || match("NOT"))
        negate();
    else
        power();

    leave("negate");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <power> -> <primary> [ "^" <negate> ] */
void power(void)
{
    enter("power");

    primary();
    if (!panicMode && match("EXPONENT"))
        negate();

    leave("power");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <primary> -> <literal>
                | IDENTIFIER [ <call-args> | <index> { <index> } ]
                | "(" <expr> ")" | <list> */
void primary(void)
{
    enter("primary");

    if (check("CONST_INT") || check("CONST_DECIMAL") || check("CONST_CHAR") ||
        check("CONST_STRING") || check("CONST_BOOL"))
    {
        literal();
    }
    else if (match("IDENTIFIER"))
    {
        if (check("LEFT_PAREN"))
            call_args();
        else
        {
            while (!panicMode && check("LEFT_BRACKET"))
                index_expr();
        }
    }
    else if (match("LEFT_PAREN"))
    {
        expr();

        if (!panicMode)
            expect("RIGHT_PAREN");
    }
    else if (check("LEFT_BRACE"))
        list();
    else
    {
        syntax_error("an expression");
    }

    leave("primary");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <arg-list> -> <expr> { "," <expr> } */
void arg_list(void)
{
    enter("arg-list");

    expr();
    while (!panicMode && match("COMMA"))
        expr();

    leave("arg-list");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <list> -> "{" [ <expr> { "," <expr> } ] "}" */
void list(void)
{
    enter("list");

    if (expect("LEFT_BRACE"))
    {
        if (!check("RIGHT_BRACE"))
        {
            expr();
            while (!panicMode && match("COMMA"))
                expr();
        }

        if (!panicMode)
            expect("RIGHT_BRACE");
    }

    leave("list");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <index> -> "[" <expr> "]" */
void index_expr(void)
{
    enter("index");

    if (expect("LEFT_BRACKET"))
    {
        expr();

        if (!panicMode)
            expect("RIGHT_BRACKET");
    }

    leave("index");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <literal> -> <number> | CONST_CHAR | CONST_STRING | CONST_BOOL */
void literal(void)
{
    enter("literal");

    if (check("CONST_INT") || check("CONST_DECIMAL"))
        number();
    else if (check("CONST_CHAR") || check("CONST_STRING") || check("CONST_BOOL"))
        advance();
    else
    {
        syntax_error("a literal");
    }

    leave("literal");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* <number> -> CONST_INT | CONST_DECIMAL */
void number(void)
{
    enter("number");

    if (check("CONST_INT") || check("CONST_DECIMAL"))
        advance();
    else
    {
        syntax_error("a number");
    }

    leave("number");
}

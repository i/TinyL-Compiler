/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Spring 2013                              *
 *  Authors: Ulrich Kremer                   *
 *           Hans Christian Woithe           *
 *********************************************
 */

/* -------------------------------------------------

   CFG for tinyL LANGUAGE

   PROGRAM ::= STMTLIST .
   STMTLIST ::= STMT MORESTMTS
   MORESTMTS ::= ; STMTLIST | epsilon
   STMT ::= ASSIGN | READ | PRINT
   ASSIGN ::= VARIABLE = EXPR
   READ ::= ? VARIABLE
   PRINT ::= ! VARIABLE
   EXPR ::= + EXPR EXPR |
   - EXPR EXPR |
 * EXPR EXPR |
 VARIABLE | 
 DIGIT
 VARIABLE ::= a | b | c | d | e 
 DIGIT ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

NOTE: tokens are exactly a single character long

Example expressions:

+12.
+1b.
+*34-78.
-*+1+2a58.

Example programs;

?a;?b;c=+3*ab;d=+c1;!d.
b=-*+1+2a58;!b.

---------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Instr.h"
#include "InstrUtils.h"
#include "Utils.h"

#define MAX_BUFFER_SIZE 500
#define EMPTY_FIELD 0xFFFFF
#define token *buffer

/* GLOBALS */
static char *buffer = NULL;	/* read buffer */
static int regnum = 1;		/* for next free virtual register number */
static FILE *outfile = NULL;	/* output of code generation */

/* Utilities */
static void CodeGen(OpCode opcode, int field1, int field2, int field3);
static inline void next_token();
static inline int next_register();
static inline int is_digit(char c);
static inline int to_digit(char c);
static inline int is_identifier(char c);
static char *read_input(FILE * f);

/* Routines for recursive descending parser LL(1) */
static void program();
static void stmtlist();
static void morestmts();
static void stmt();
static void assign();
static void read();
static void print();
static int expr();
static int variable();
static int digit();

/*************************************************************************/
/* Definitions for recursive descending parser LL(1)                     */
/*************************************************************************/
static int digit()
{
    int reg;

    if (!is_digit(token)) {
        ERROR("Expected digit\n");
        exit(EXIT_FAILURE);
    }
    reg = next_register();
    CodeGen(LOADI, reg, to_digit(token), EMPTY_FIELD);
    next_token();
    return reg;
}

static int variable()
{
    int reg;

    if(!is_identifier(token)) {
        printf("token: %c\n", token);
        ERROR("Expected identifier\n");
        exit(EXIT_FAILURE);
    }
    reg = next_register();
    /* YOUR CODE GOES HERE */
    next_token();
    return reg;
}

static int expr()
{
    int reg, left_reg, right_reg;

    switch (token) {
        case '+':
            next_token();
            left_reg = expr();
            right_reg = expr();
            reg = next_register();
            CodeGen(ADD, reg, left_reg, right_reg);
            return reg;

        case '-':
            next_token();
            left_reg = expr();
            right_reg = expr();
            reg = next_register();
            CodeGen(SUB, reg, left_reg, right_reg);
            return reg;

        case '*':
            next_token();
            left_reg = expr();
            right_reg = expr();
            reg = next_register();
            CodeGen(MUL, reg, left_reg, right_reg);
            return reg;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
            return variable();

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return digit();
        default:
            ERROR("Symbol %c unknown\n", token);
            exit(EXIT_FAILURE);
    }
}

static void assign()
{
    int reg, val;
    char id;

    reg = next_register();
    id = variable();
    next_token();
    if(token != '='){
        ERROR("Expected '=' but got %c instead\n", token);
        exit(EXIT_FAILURE);
    }
    next_token();
    CodeGen(LOADI, reg, val, EMPTY_FIELD);
    CodeGen(STORE, id, val, EMPTY_FIELD);

}

static void read()
{
    char id;

    next_token();
    if(!is_identifier(token)){
        ERROR("Expected identifier\n");
        exit(EXIT_FAILURE);
    }
    id = token;
    CodeGen(READ, id, EMPTY_FIELD, EMPTY_FIELD);
}

static void print()
{
    char id;

    id = variable();
    if(!is_identifier(token)){
        ERROR("Expected identifier\n");
        exit(EXIT_FAILURE);
    }
    CodeGen(STORE, id, EMPTY_FIELD, EMPTY_FIELD);
    CodeGen(READ, id, EMPTY_FIELD, EMPTY_FIELD);
}

static void stmt()
{
    switch(token){
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
            assign();
            return;

        case '?':
            read();
            return;

        case '!':
            print();
            return;

        default:
            ERROR("Symbol %c unknown\n", token);
            exit(EXIT_FAILURE);
    }
/*    next_token()??*/
}

static void morestmts()
{
    switch(token){
        case ';':
            next_token();
            stmtlist();
            return;

        case '.':
            return;

        default:
            ERROR("Symbol %c unknown\n", token);
            exit(EXIT_FAILURE);
    }
}

static void stmtlist()
{
    switch(token){
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case '?':
        case '!':
            stmt();

        case ';':
            next_token();
            morestmts();

        default:
            ERROR("Symbol %c unknown\n", token);
            exit(EXIT_FAILURE);
    }
}

static void program()
{
    stmtlist();
    if (token != '.') {
        ERROR("Program error.  Current input symbol is %c\n", token);
        exit(EXIT_FAILURE);
    };
}

/*************************************************************************/
/* Utility definitions                                                   */
/*************************************************************************/
static void CodeGen(OpCode opcode, int field1, int field2, int field3)
{
    Instruction instr;

    if (!outfile) {
        ERROR("File error\n");
        exit(EXIT_FAILURE);
    }
    instr.opcode = opcode;
    instr.field1 = field1;
    instr.field2 = field2;
    instr.field3 = field3;
    PrintInstruction(outfile, &instr);
}

static inline void next_token()
{
    printf("%c ", *buffer);
    if (*buffer == ';')
        printf("\n");
    buffer++;
    if (*buffer == '.')
        printf(".\n");
}

static inline int next_register()
{
    return regnum++;
}

static inline int is_digit(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    return 0;
}

static inline int to_digit(char c)
{
    if (is_digit(c))
        return c - '0';
    WARNING("Non-digit passed to %s, returning zero\n", __func__);
    return 0;
}

static inline int is_identifier(char c)
{
    if (c >= 'a' && c <= 'e')
        return 1;
    return 0;
}

static char * read_input(FILE * f)
{
    int max, i, c;
    char *b;

    max = MAX_BUFFER_SIZE;
    b = (char *)calloc(max, sizeof(char));
    if (!b) {
        ERROR("Calloc failed\n");
        exit(EXIT_FAILURE);
    }

    /* skip leading whitespace */
    for (;;) {
        c = fgetc(f);
        if (EOF == c) {
            break;
        } else if (!isspace(c)) {
            ungetc(c, f);
            break;
        }
    }

    i = 0;
    for (;;) {
        c = fgetc(f);
        if (EOF == c) {
            b[i] = '\0';
            break;
        }
        b[i] = c;
        if (max - 1 == i) {
            max = max + max;
            b = (char *)realloc(buffer, max * sizeof(char));
            if (!b) {
                ERROR("Realloc failed\n");
                exit(EXIT_FAILURE);
            }
        }
        ++i;
    }
    return b;
}

/*************************************************************************/
/* Main function                                                         */
/*************************************************************************/

int main(int argc, char *argv[])
{
    const char *outfilename = "tinyL.out";
    char *input;
    FILE *infile;

    printf("------------------------------------------------\n");
    printf("CS314 compiler for tinyL\n");
    printf("------------------------------------------------\n");

    if (argc != 2) {
        fprintf(stderr, "Use of command:\n  compile <tinyL file>\n");
        exit(EXIT_FAILURE);
    }

    infile = fopen(argv[1], "r");
    if (!infile) {
        ERROR("Cannot open input file \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    outfile = fopen(outfilename, "w");
    if (!outfile) {
        ERROR("Cannot open output file \"%s\"\n", outfilename);
        exit(EXIT_FAILURE);
    }

    input = read_input(infile);
    buffer = input;
    program();

    printf("\nCode written to file \"%s\".\n\n", outfilename);

    free(input);
    fclose(infile);
    fclose(outfile);
    return EXIT_SUCCESS;
}

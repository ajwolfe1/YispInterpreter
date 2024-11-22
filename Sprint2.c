// Sprint 1: Basic S-Expression Parsing and REPL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define SExpr types: atom, symbol, list
typedef enum { ATOM, SYMBOL, CONS } SExprType;

typedef struct SExpr {
    SExprType type;
    union {
        char *atom;          // For atoms/symbols
        struct {
            struct SExpr *car;
            struct SExpr *cdr;
        };                   // For lists
    };
} SExpr;

// Create S-expression constructors
SExpr* makeAtom(char* atom) {
    SExpr* expr = (SExpr*)malloc(sizeof(SExpr));
    expr->type = ATOM;
    expr->atom = strdup(atom);
    return expr;
}

SExpr* cons(SExpr* car, SExpr* cdr) {
    SExpr* expr = (SExpr*)malloc(sizeof(SExpr));
    expr->type = CONS;
    expr->car = car;
    expr->cdr = cdr;
    return expr;
}

// Print S-expression
void printSExpr(SExpr* expr) {
    if (expr->type == ATOM) {
        printf("%s", expr->atom);
    } else if (expr->type == CONS) {
        printf("(");
        printSExpr(expr->car);
        printf(" . ");
        printSExpr(expr->cdr);
        printf(")");
    }
}

// Parse function placeholder
SExpr* parse(char* input) {
    // Stub: pretend the input is an atom for now
    return makeAtom(input);
}

// Eval placeholder
SExpr* eval(SExpr* expr) {
    return expr;
}

// REPL function
void repl() {
    char input[256];
    while (1) {
        printf("> ");
        fgets(input, sizeof(input), stdin);
        SExpr* parsed = parse(input);
        SExpr* result = eval(parsed);
        printSExpr(result);
        printf("\n");
    }
}

// Sprint 2: Nil, Truth, Predicate Functions

SExpr* nil;
SExpr* truth;

SExpr* makeTruth() {
    truth = makeAtom("t");
    return truth;
}

int isNil(SExpr* expr) {
    return expr == nil;
}

int isSymbol(SExpr* expr) {
    return expr->type == ATOM && !strcmp(expr->atom, "symbol");
}

int isNumber(SExpr* expr) {
    return expr->type == ATOM && isdigit(expr->atom[0]);
}

int isList(SExpr* expr) {
    return expr->type == CONS;
}

SExpr* car(SExpr* expr) {
    return expr->car;
}

SExpr* cdr(SExpr* expr) {
    return expr->cdr;
}

// Constructors for numbers/symbols
SExpr* makeSymbol(char* sym) {
    return makeAtom(sym);
}

SExpr* makeNumber(char* num) {
    return makeAtom(num);
}

// Testing Sprint 2
void testSprint2() {
    nil = makeAtom("nil");
    truth = makeTruth();

    // Testing predicates
    printSExpr(nil); printf("\n");
    printSExpr(truth); printf("\n");
    printSExpr(makeSymbol("symbol")); printf("\n");
    printSExpr(makeNumber("411")); printf("\n");

    SExpr* list = cons(makeSymbol("one"), cons(makeSymbol("two"), cons(makeSymbol("three"), nil)));
    printSExpr(list); printf("\n");
}

int main() {
    testSprint2();
    return 0;
}


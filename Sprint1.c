// Sprint 1: Basic S-Expression Parsing and REPL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    SExpr* expr = malloc(sizeof(SExpr));
    expr->type = ATOM;
    expr->atom = strdup(atom);
    return expr;
}

SExpr* cons(SExpr* car, SExpr* cdr) {
    SExpr* expr = malloc(sizeof(SExpr));
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

int main() {
    repl();
    return 0;
}

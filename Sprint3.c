#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define SExpr types: atom, symbol, list, and number
typedef enum { ATOM, SYMBOL, CONS, NUMBER } SExprType;

typedef struct SExpr {
    SExprType type;
    union {
        char *atom;          // For atoms and symbols
        int number;          // For numbers
        struct {
            struct SExpr *car;
            struct SExpr *cdr;
        };                   // For lists (CONS)
    };
} SExpr;

// Global nil and truth variables
SExpr* nil;
SExpr* truth;

// Create S-expression constructors
SExpr* makeAtom(char* atom) {
    SExpr* expr = (SExpr*)malloc(sizeof(SExpr));
    expr->type = ATOM;
    expr->atom = strdup(atom);
    return expr;
}

SExpr* makeNumber(int num) {
    SExpr* expr = (SExpr*)malloc(sizeof(SExpr));
    expr->type = NUMBER;
    expr->number = num;
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
    } else if (expr->type == NUMBER) {
        printf("%d", expr->number);
    } else if (expr->type == CONS) {
        printf("(");
        printSExpr(expr->car);
        printf(" . ");
        printSExpr(expr->cdr);
        printf(")");
    }
}

// Arithmetic functions
SExpr* add(SExpr* a, SExpr* b) {
    if (a->type == NUMBER && b->type == NUMBER) {
        return makeNumber(a->number + b->number);
    }
    return nil;
}

SExpr* sub(SExpr* a, SExpr* b) {
    if (a->type == NUMBER && b->type == NUMBER) {
        return makeNumber(a->number - b->number);
    }
    return nil;
}

// Relational functions
SExpr* eq(SExpr* a, SExpr* b) {
    if (a->type == NUMBER && b->type == NUMBER) {
        return (a->number == b->number) ? truth : nil;
    }
    return nil;
}

SExpr* lt(SExpr* a, SExpr* b) {
    if (a->type == NUMBER && b->type == NUMBER) {
        return (a->number < b->number) ? truth : nil;
    }
    return nil;
}

// Initialize nil and truth
void initializeSpecialValues() {
    nil = makeAtom("nil");    // Nil as atom "nil"
    truth = makeAtom("t");     // Truth as atom "t"
}

// Test Sprint 3
void testSprint3() {
    printSExpr(add(makeNumber(2), makeNumber(3))); printf("\n");  // Expected: 5
    printSExpr(sub(makeNumber(5), makeNumber(3))); printf("\n");  // Expected: 2
    printSExpr(eq(makeNumber(2), makeNumber(2))); printf("\n");   // Expected: t
    printSExpr(lt(makeNumber(2), makeNumber(3))); printf("\n");   // Expected: t
}

int main() {
    initializeSpecialValues();  // Initialize nil and truth
    testSprint3();
    return 0;
}

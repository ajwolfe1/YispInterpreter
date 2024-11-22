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
SExpr* makeAtom(const char* atom) {
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

// Additional functions
int isNumber(SExpr* expr) {
    return expr->type == NUMBER;
}

int isNil(SExpr* expr) {
    return expr == nil;
}

int isSymbol(SExpr* expr) {
    return expr->type == ATOM && expr->atom != NULL;
}

SExpr* car(SExpr* expr) {
    if (expr->type == CONS) {
        return expr->car;
    }
    return nil;
}

SExpr* cdr(SExpr* expr) {
    if (expr->type == CONS) {
        return expr->cdr;
    }
    return nil;
}

SExpr* makeSymbol(const char* sym) {
    return makeAtom(sym);
}

// Placeholder for the lookup function
SExpr* lookup(SExpr* expr) {
    // Implement symbol lookup in the environment later
    return nil;
}

// Sprint 4 code...
SExpr* eval(SExpr* expr) {
    if (isNumber(expr) || isNil(expr)) {
        return expr;
    }
    if (isSymbol(expr)) {
        return lookup(expr);  // Implement this later
    }

    SExpr* first = car(expr);
    if (isSymbol(first)) {
        if (strcmp(first->atom, "add") == 0) {
            return add(eval(car(cdr(expr))), eval(car(cdr(cdr(expr)))));
        }
        if (strcmp(first->atom, "sub") == 0) {
            return sub(eval(car(cdr(expr))), eval(car(cdr(cdr(expr)))));
        }
    }
    return expr;  // Default fallback
}

// Test Sprint 4
void testSprint4() {
    SExpr* expr = cons(makeSymbol("add"), cons(makeNumber(1), cons(makeNumber(2), nil)));
    printSExpr(eval(expr));  // Should print 3
}

int main() {
    initializeSpecialValues();  // Initialize nil and truth
    testSprint4();
    return 0;
}

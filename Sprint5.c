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

SExpr* makeSymbol(const char* symbol) {
    SExpr* expr = (SExpr*)malloc(sizeof(SExpr));
    expr->type = SYMBOL;
    expr->atom = strdup(symbol);
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
        if (expr->cdr->type == CONS) {
            printf(" ");
            printSExpr(expr->cdr);
        } else {
            printf(" . ");
            printSExpr(expr->cdr);
        }
        printf(")");
    }
}

// Global environment for variables
typedef struct Environment {
    SExpr* symbol;
    SExpr* value;
    struct Environment* next;
} Environment;

Environment* globalEnv = NULL;

// Function to add to the environment
void addToEnv(SExpr* symbol, SExpr* value) {
    Environment* newEnv = (Environment*)malloc(sizeof(Environment));
    newEnv->symbol = symbol;
    newEnv->value = value;
    newEnv->next = globalEnv;
    globalEnv = newEnv;
}

// Function to lookup a symbol in the environment
SExpr* lookup(SExpr* symbol) {
    Environment* current = globalEnv;
    while (current) {
        if (strcmp(current->symbol->atom, symbol->atom) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return nil;  // Not found
}

// Arithmetic functions
SExpr* lisp_add(SExpr* a, SExpr* b) {
    if (a->type == NUMBER && b->type == NUMBER) {
        return makeNumber(a->number + b->number);
    }
    return nil;
}

SExpr* lisp_sub(SExpr* a, SExpr* b) {
    if (a->type == NUMBER && b->type == NUMBER) {
        return makeNumber(a->number - b->number);
    }
    return nil;
}

// Logical functions
SExpr* lisp_and(SExpr* a, SExpr* b) {
    return (a->type == ATOM && strcmp(a->atom, "nil") == 0) ? nil : b;
}

SExpr* lisp_or(SExpr* a, SExpr* b) {
    return (a->type == ATOM && strcmp(a->atom, "nil") != 0) ? a : b;
}

// Initialize nil and truth
void initializeSpecialValues() {
    nil = makeAtom("nil");    // Nil as atom "nil"
    truth = makeAtom("t");    // Truth as atom "t"
}

// Main eval function
SExpr* eval(SExpr* expr) {
    if (expr == nil) {
        return nil;
    }
    
    if (expr->type == NUMBER || expr->type == ATOM) {
        return expr;
    }
    
    if (expr->type == SYMBOL) {
        return lookup(expr);
    }

    // Handle arithmetic and logical function calls
    SExpr* first = expr->car;
    if (first->type == SYMBOL) {
        if (strcmp(first->atom, "add") == 0) {
            return lisp_add(eval(expr->cdr->car), eval(expr->cdr->cdr->car));
        } else if (strcmp(first->atom, "sub") == 0) {
            return lisp_sub(eval(expr->cdr->car), eval(expr->cdr->cdr->car));
        } else if (strcmp(first->atom, "and") == 0) {
            return lisp_and(eval(expr->cdr->car), eval(expr->cdr->cdr->car));
        } else if (strcmp(first->atom, "or") == 0) {
            return lisp_or(eval(expr->cdr->car), eval(expr->cdr->cdr->car));
        }
    }

    return expr;  // Default fallback
}

// Test functions for Sprint 5
void testSprint5() {
    // Test adding numbers
    SExpr* addCallExpr = cons(makeSymbol("add"), cons(makeNumber(3), cons(makeNumber(5), nil)));
    printf("Add Result: ");
    printSExpr(eval(addCallExpr));  // Should print 8
    printf("\n");

    // Test subtracting numbers
    SExpr* subtractCallExpr = cons(makeSymbol("sub"), cons(makeNumber(10), cons(makeNumber(4), nil)));
    printf("Subtract Result: ");
    printSExpr(eval(subtractCallExpr));  // Should print 6
    printf("\n");

    // Test logical functions
    SExpr* andCallExpr = cons(makeSymbol("and"), cons(makeAtom("t"), cons(nil, nil)));
    printf("And Result: ");
    printSExpr(eval(andCallExpr));  // Should print nil
    printf("\n");

    SExpr* orCallExpr = cons(makeSymbol("or"), cons(nil, cons(makeAtom("t"), nil)));
    printf("Or Result: ");
    printSExpr(eval(orCallExpr));  // Should print t
    printf("\n");
}

int main() {
    initializeSpecialValues();  // Initialize nil and truth
    testSprint5();
    return 0;
}

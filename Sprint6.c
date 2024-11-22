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
    if (expr == nil) {
        printf("nil");
        return;
    }

    if (expr->type == ATOM || expr->type == SYMBOL) {
        printf("%s", expr->atom);
    } else if (expr->type == NUMBER) {
        printf("%d", expr->number);
    } else if (expr->type == CONS) {
        printf("(");
        printSExpr(expr->car);
        SExpr* cdr = expr->cdr;
        while (cdr != nil) {
            printf(" ");
            printSExpr(cdr->car);
            cdr = cdr->cdr;
        }
        printf(")");
    }
}

// Global environment for storing defined functions
typedef struct Env {
    SExpr* symbol;  // The name of the function
    SExpr* args;    // The argument list
    SExpr* body;    // The function body
    struct Env* next;
} Env;

Env* globalEnv = NULL;

// Initialize nil and truth
void initializeSpecialValues() {
    nil = makeAtom("nil");    // Nil as atom "nil"
    truth = makeAtom("t");     // Truth as atom "t"
}

// Function to define a new function in the environment
void defineFunction(SExpr* expr) {
    SExpr* funcName = expr->cdr->car;
    SExpr* args = expr->cdr->cdr->car;
    SExpr* body = expr->cdr->cdr->cdr->car;

    Env* newEnv = (Env*)malloc(sizeof(Env));
    newEnv->symbol = funcName;
    newEnv->args = args;
    newEnv->body = body;
    newEnv->next = globalEnv;  // Link to previous environment
    globalEnv = newEnv;  // Add new environment to the top of the stack
}

// Lookup function in the environment
Env* lookup(SExpr* symbol) {
    for (Env* env = globalEnv; env != NULL; env = env->next) {
        if (strcmp(env->symbol->atom, symbol->atom) == 0) {
            return env; // Return the entire environment entry
        }
    }
    return NULL; // Not found
}

// Evaluate S-expression
SExpr* eval(SExpr* expr) {
    if (expr == nil) {
        return nil;
    }
    
    if (expr->type == NUMBER || expr->type == ATOM) {
        return expr;
    }
    
    if (expr->type == SYMBOL) {
        Env* envEntry = lookup(expr);
        return envEntry ? envEntry->body : nil; // Lookup in global environment
    }

    // Handle function definitions
    if (expr->type == CONS) {
        SExpr* first = expr->car;
        if (first->type == SYMBOL && strcmp(first->atom, "define") == 0) {
            defineFunction(expr);
            return nil; // Defined but no value to return
        }
    }

    // Handle function calls
    SExpr* first = expr->car;
    if (first->type == SYMBOL) {
        // Check for built-in functions
        if (strcmp(first->atom, "add") == 0) {
            SExpr* arg1 = eval(expr->cdr->car);
            SExpr* arg2 = eval(expr->cdr->cdr->car);
            return makeNumber(arg1->number + arg2->number);
        } else if (strcmp(first->atom, "subtract") == 0) {
            SExpr* arg1 = eval(expr->cdr->car);
            SExpr* arg2 = eval(expr->cdr->cdr->car);
            return makeNumber(arg1->number - arg2->number);
        } else {
            // Handle user-defined functions
            Env* funcEntry = lookup(first);
            if (funcEntry != NULL) {
                SExpr* args = funcEntry->args;
                SExpr* body = funcEntry->body;

                // Evaluate arguments
                SExpr* argValues = nil;
                SExpr* argExpr = expr->cdr;
                while (argExpr != nil) {
                    argValues = cons(eval(argExpr->car), argValues);
                    argExpr = argExpr->cdr;
                }
                
                // Reverse argValues to maintain order
                SExpr* prev = nil;
                SExpr* current = argValues;
                while (current != nil) {
                    prev = cons(current->car, prev);
                    current = current->cdr;
                }
                argValues = prev;

                // Create new environment for the function call
                Env* newEnv = (Env*)malloc(sizeof(Env));
                newEnv->next = globalEnv;  // Link to previous environment
                globalEnv = newEnv;  // Add new environment to the top of the stack

                // Bind arguments to their corresponding values
                SExpr* argList = args;
                SExpr* valueList = argValues;
                while (argList != nil && valueList != nil) {
                    newEnv->symbol = argList->car;
                    newEnv->body = valueList->car; // Set argument value
                    argList = argList->cdr;
                    valueList = valueList->cdr;
                }

                // Evaluate the function body
                SExpr* result = eval(body);
                globalEnv = globalEnv->next;  // Pop the environment
                return result;  // Return the result of the function body
            }
        }
    }

    return expr;  // Default fallback
}

// Test functions
void testSprint5() {
    // Define a function for addition
    SExpr* defAddExpr = cons(makeSymbol("define"),
        cons(makeSymbol("add"),
            cons(cons(makeSymbol("a"),
                cons(makeSymbol("b"), nil)), // Arguments
            cons(cons(makeSymbol("+"),
                cons(makeSymbol("a"),
                cons(makeSymbol("b"), nil))), // Body: (+ a b)
            nil)) // Close the function definition
        )
    );

    eval(defAddExpr);  // Define the addition function

    // Test calling the add function
    SExpr* addCallExpr = cons(makeSymbol("add"), cons(makeNumber(3), cons(makeNumber(5), nil)));
    printf("User-defined Add Result: ");
    printSExpr(eval(addCallExpr));  // Should print 8
    printf("\n");
}

void testSprint6() {
    // Define a function for subtraction
    SExpr* defSubExpr = cons(makeSymbol("define"),
        cons(makeSymbol("subtract"),
            cons(cons(makeSymbol("a"),
                cons(makeSymbol("b"), nil)), // Arguments
            cons(cons(makeSymbol("-"),
                cons(makeSymbol("a"),
                cons(makeSymbol("b"), nil))), // Body: (- a b)
            nil)) // Close the function definition
        )
    );

    eval(defSubExpr);  // Define the subtraction function

    // Test calling the subtract function
    SExpr* subtractCallExpr = cons(makeSymbol("subtract"), cons(makeNumber(10), cons(makeNumber(4), nil)));
    printf("User-defined Subtract Result: ");
    printSExpr(eval(subtractCallExpr));  // Should print 6
    printf("\n");
}

int main() {
    initializeSpecialValues();
    
    testSprint5();
    testSprint6();

    return 0;
}

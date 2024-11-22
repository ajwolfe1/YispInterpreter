#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SExpr {
    enum { SYMBOL, NUMBER, CONS, NIL, ERROR, LAMBDA } type;
    union {
        char* symbol;
        int number;
        struct {
            struct SExpr* car;
            struct SExpr* cdr;
            struct SExpr* params;
            struct SExpr* body;
            struct SExpr* env;
        };
    };
} SExpr;


typedef struct Env {
    SExpr* name;
    SExpr* value;
    struct Env* next;
} Env;

Env* global_env = NULL;

SExpr* nil;
SExpr* makeSymbol(char* name);
SExpr* makeNumber(int value);
SExpr* cons(SExpr* car, SExpr* cdr);
SExpr* eval(SExpr* expr);
SExpr* makeError(char* message);  // Declaration of makeError function

// Helper to compare two SExprs for equality
int isTruthy(SExpr* expr) {
    return expr != nil && (expr->type != NIL && (expr->type != NUMBER || expr->number != 0));
}

void initNil() {
    if (nil == NULL) {
        nil = (SExpr*)malloc(sizeof(SExpr));
        nil->type = NIL;
    }
}

SExpr* makeSymbol(char* name) {
    SExpr* s = (SExpr*)malloc(sizeof(SExpr));
    s->type = SYMBOL;
    s->symbol = name;
    return s;
}

SExpr* makeNumber(int value) {
    SExpr* n = (SExpr*)malloc(sizeof(SExpr));
    n->type = NUMBER;
    n->number = value;
    return n;
}

SExpr* cons(SExpr* car, SExpr* cdr) {
    SExpr* c = (SExpr*)malloc(sizeof(SExpr));
    if (!c) {
        printf("Memory allocation failed for cons\n");
        exit(1);  // Handle memory allocation failure gracefully
    }
    c->type = CONS;
    c->car = car;
    c->cdr = cdr;
    return c;
}


// The `makeError` function now returns an SExpr* to correctly handle errors
SExpr* makeError(char* message) {
    SExpr* e = (SExpr*)malloc(sizeof(SExpr));
    e->type = SYMBOL;
    e->symbol = message;
    return e;
}

// Arithmetic operations
SExpr* evalAdd(SExpr* expr) {
    return makeNumber(eval(expr->car)->number + eval(expr->cdr->car)->number);
}

SExpr* evalSubtract(SExpr* expr) {
    return makeNumber(eval(expr->car)->number - eval(expr->cdr->car)->number);
}

SExpr* evalMultiply(SExpr* expr) {
    return makeNumber(eval(expr->car)->number * eval(expr->cdr->car)->number);
}

SExpr* evalDivide(SExpr* expr) {
    int denominator = eval(expr->cdr->car)->number;
    if (denominator == 0) return nil;
    return makeNumber(eval(expr->car)->number / denominator);
}

// Logical operations
SExpr* evalAnd(SExpr* expr) {
    SExpr* e1 = eval(expr->car);
    if (!isTruthy(e1)) return nil;
    return eval(expr->cdr->car);
}

SExpr* evalOr(SExpr* expr) {
    SExpr* e1 = eval(expr->car);
    if (isTruthy(e1)) return e1;
    return eval(expr->cdr->car);
}

// Conditional `if` expression
SExpr* evalIf(SExpr* expr) {
    SExpr* condition = eval(expr->car);
    if (isTruthy(condition)) return eval(expr->cdr->car);
    return eval(expr->cdr->cdr->car);
}

// The `cond` construct evaluation
SExpr* evalCond(SExpr* expr) {
    // printSExpr(exprIn->car->cdr->car);
    // SExpr* expr = exprIn->car->cdr;
    // printSExpr(expr->car);
    while (expr != nil) {
        // Check that `expr` is a cons cell
        // printf("Entered");
        // printf("\n");
        if (expr->type != CONS) {
            printf("Not CONS");
            return makeError("COND: Malformed clause list - expected cons cell");
        }
        
        SExpr* pair = expr->car;  // Extract the current clause
        if (pair->type != CONS) {
            printf("Pair not CONS");
            return makeError("COND: Each clause must be a cons cell with a condition and result");
        }

        // Extract the condition
        SExpr* condition = pair->car;
        // printSExpr(pair->car->car);
        // printf("\n");
        // printSExpr(pair->cdr->car);
        if (condition == nil) {
            printf("Wrong COND");
            return makeError("COND: Missing condition in clause");
        }

        // Extract the result
        if (pair->cdr == nil) { // || pair->cdr->type != CONS) {
            printf("Wrong Result");
            return makeError("COND: Missing or malformed result in clause");
        }
        
        SExpr* result = pair->cdr;
        // printSExpr(result);
        // printSExpr(pair->car->cdr);
        // printf("\n");
        // Evaluate the condition
        SExpr* eval_condition = eval(condition);
        // printSExpr(condition->car);
        if (eval_condition == nil) {
            // printSExpr(eval_condition);
            // printf("\n");
            // printf("Fail Eval");
            // return makeError("COND: Error evaluating condition");
        }

        // Check if the condition is truthy
        if (eval_condition != nil) { // && eval_condition->type != NIL) {
            // Evaluate and return the result
            SExpr* eval_result = eval(result);
            // printSExpr(eval_result);
            if (eval_result == nil) {
                return makeError("COND: Error evaluating result");
            }
            return eval_result;
        }

        // Move to the next condition-result pair
        // printSExpr(expr->car->cdr);
        // printf("\n");
        expr = expr->car->cdr;
        // printSExpr(expr);
        // printf("\n");
    }

    return nil;  // If no condition is truthy, return nil
}

// Comparison operations
SExpr* evalGreaterThan(SExpr* expr) {
    return eval(expr->car)->number > eval(expr->cdr->car)->number ? makeSymbol("t") : nil;
}

SExpr* evalLessThan(SExpr* expr) {
    return eval(expr->car)->number < eval(expr->cdr->car)->number ? makeSymbol("t") : nil;
}

SExpr* evalGreaterEqual(SExpr* expr) {
    return eval(expr->car)->number >= eval(expr->cdr->car)->number ? makeSymbol("t") : nil;
}

SExpr* evalLessEqual(SExpr* expr) {
    return eval(expr->car)->number <= eval(expr->cdr->car)->number ? makeSymbol("t") : nil;
}

void set(SExpr* name, SExpr* value) {
    // Ensure name is a symbol
    if (name->type != SYMBOL) {
        fprintf(stderr, "Error: Name must be a symbol\n");
        return;
    }

    // Check if the symbol already exists in the environment
    Env* current = global_env;
    while (current) {
        if (strcmp(current->name->symbol, name->symbol) == 0) {
            current->value = value; // Update value
            return;
        }
        current = current->next;
    }

    // Add a new entry to the environment
    Env* new_entry = malloc(sizeof(Env));
    if (!new_entry) {
        fprintf(stderr, "Error: Memory allocation failed for environment entry\n");
        return;
    }
    new_entry->name = name;
    new_entry->value = value;
    new_entry->next = global_env;
    global_env = new_entry;
}

SExpr* get(SExpr* name) {
    // Ensure name is a symbol
    if (name->type != SYMBOL) {
        fprintf(stderr, "Error: Name must be a symbol\n");
        return nil;
    }

    // Search for the symbol in the environment
    Env* current = global_env;
    while (current) {
        if (strcmp(current->name->symbol, name->symbol) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return nil; // Symbol not found
}

SExpr* eq(SExpr* a, SExpr* b) {
    // printf(a->number);
    // printf(" ");
    // printf(b->number);
    // printf("\n");
    // Compare numbers
    if (a->type == NUMBER && b->type == NUMBER) {
        // printf("Is Number");
        // printf("\n");
        return a->number == b->number ? makeSymbol("t") : nil;
    }

    // Compare symbols
    if (a->type == SYMBOL && b->type == SYMBOL) {
        // printf("Is Symbol");
        // printf("\n");
        return strcmp(a->symbol, b->symbol) == 0 ? makeSymbol("t") : nil;
    }

    // Types mismatch
    return nil;
}

// Evaluation function for all expressions
SExpr* eval(SExpr* expr) {
    if (expr == nil) return nil;
    if (expr->type == NUMBER || expr->type == NIL) return expr;
    if (expr->type == SYMBOL) {
        if (strcmp(expr->symbol, "t") == 0) return makeSymbol("t");
        return nil;
    }
    SExpr* function = expr->car;  // First element
    SExpr* args = expr->cdr;  
    if (strcmp(function->symbol, "quote") == 0) {
        if (args == nil || args->type != CONS) {
            return makeError("QUOTE: Missing or malformed argument");
        }
        return args->car; // Return cadr (car of cdr)
    }
    if (strcmp(function->symbol, "set") == 0) {
        if (args == nil || args->type != CONS || args->cdr == nil || args->cdr->cdr == nil) {
            return makeError("SET: Missing or malformed arguments");
        }
        SExpr* name = args->car;         // First argument
        SExpr* valueExpr = args->cdr->car; // Second argument (value)
        
        if (name->type != SYMBOL) {
            return makeError("SET: First argument must be a symbol");
        }
        
        SExpr* value = eval(valueExpr); // Evaluate value
        if (value == nil) {
            return makeError("SET: Error evaluating value");
        }

        set(name, value); // Store in environment
        return value;     // Return the evaluated value
    }
    if (strcmp(function->symbol, "eq") == 0) {
        // printSExpr(args->car);
        // printf("\n");
        // printSExpr(args->cdr->car);
        // printf("\n");
        SExpr* arg1 = eval(args->car);
        SExpr* arg2 = eval(args->cdr->car);
        return eq(arg1, arg2);
    }
    if (strcmp(function->symbol, "lambda") == 0) {
        SExpr* params = args->car;
        SExpr* body = args->cdr->car;

        // Create the lambda
        SExpr* lambda = malloc(sizeof(SExpr));
        lambda->type = LAMBDA;
        lambda->params = params;
        lambda->body = body;
        // lambda->env = current_env; // Save the closure's environment
        return lambda;
    }
    if (expr->type == CONS) {
        SExpr* first = expr->car;
        if (first->type == SYMBOL) {
            if (strcmp(first->symbol, "add") == 0) return evalAdd(expr->cdr);
            if (strcmp(first->symbol, "sub") == 0) return evalSubtract(expr->cdr);
            if (strcmp(first->symbol, "mul") == 0) return evalMultiply(expr->cdr);
            if (strcmp(first->symbol, "div") == 0) return evalDivide(expr->cdr);
            if (strcmp(first->symbol, "and") == 0) return evalAnd(expr->cdr);
            if (strcmp(first->symbol, "or") == 0) return evalOr(expr->cdr);
            if (strcmp(first->symbol, "if") == 0) return evalIf(expr->cdr);
            if (strcmp(first->symbol, "cond") == 0) return evalCond(expr->cdr);
            if (strcmp(first->symbol, ">") == 0) return evalGreaterThan(expr->cdr);
            if (strcmp(first->symbol, "<") == 0) return evalLessThan(expr->cdr);
            if (strcmp(first->symbol, ">=") == 0) return evalGreaterEqual(expr->cdr);
            if (strcmp(first->symbol, "<=") == 0) return evalLessEqual(expr->cdr);
        }
    }
    return nil;
}

void printSExpr(SExpr* expr) {
    if (expr == nil || expr == NULL) {
        printf("nil");
        return;
    }

    switch (expr->type) {
        case SYMBOL:
            printf("%s", expr->symbol);
            break;

        case NUMBER:
            printf("%d", expr->number);
            break;

        case CONS: {
            printf("(");
            SExpr* current = expr;
            while (current != nil && current->type == CONS) {
                printSExpr(current->car);
                current = current->cdr;
                if (current != nil && current->type == CONS) {
                    printf(" ");
                }
            }
            if (current != nil && current != nil) {
                printf(" . ");
                printSExpr(current);  // Print the improper list tail
            }
            printf(")");
            break;
        }

        case NIL:
            printf("nil");
            break;

        default:
            printf("Unknown");
            break;
    }
}

void runTests() {
    FILE* outFile = fopen("TestOutput.txt", "w"); // Open TestOutput file for writing
    if (!outFile) {
        printf("Error: Could not open TestOutput for writing\n");
        return;
    }

    fprintf(outFile, "== Extensive Testing ==\n");

    // Arithmetic Tests
    fprintf(outFile, "Test 1 (Add 2 + 3): %s\n", 
        eval(cons(makeSymbol("add"), cons(makeNumber(2), cons(makeNumber(3), nil))))->number == 5 ? "pass" : "fail");
    fprintf(outFile, "Test 2 (Subtract 5 - 3): %s\n", 
        eval(cons(makeSymbol("sub"), cons(makeNumber(5), cons(makeNumber(3), nil))))->number == 2 ? "pass" : "fail");
    fprintf(outFile, "Test 3 (Multiply 4 * 3): %s\n", 
        eval(cons(makeSymbol("mul"), cons(makeNumber(4), cons(makeNumber(3), nil))))->number == 12 ? "pass" : "fail");
    fprintf(outFile, "Test 4 (Divide 10 / 2): %s\n", 
        eval(cons(makeSymbol("div"), cons(makeNumber(10), cons(makeNumber(2), nil))))->number == 5 ? "pass" : "fail");
    fprintf(outFile, "Test 5 (Divide by zero): %s\n", 
        eval(cons(makeSymbol("div"), cons(makeNumber(10), cons(makeNumber(0), nil)))) == nil ? "pass" : "fail");

    // Logical Tests
    fprintf(outFile, "Test 6 (true && true): %s\n", 
        eval(cons(makeSymbol("and"), cons(makeSymbol("t"), cons(makeSymbol("t"), nil)))) != nil ? "pass" : "fail");
    fprintf(outFile, "Test 7 (true && false): %s\n", 
        eval(cons(makeSymbol("and"), cons(makeSymbol("t"), cons(nil, nil)))) == nil ? "pass" : "fail");
    fprintf(outFile, "Test 8 (false || true): %s\n", 
        eval(cons(makeSymbol("or"), cons(nil, cons(makeSymbol("t"), nil)))) != nil ? "pass" : "fail");

    // Comparison Tests
    fprintf(outFile, "Test 9 (Greater Than 5 > 3): %s\n", 
        eval(cons(makeSymbol(">"), cons(makeNumber(5), cons(makeNumber(3), nil)))) != nil ? "pass" : "fail");
    fprintf(outFile, "Test 10 (Less Than 2 < 5): %s\n", 
        eval(cons(makeSymbol("<"), cons(makeNumber(2), cons(makeNumber(5), nil)))) != nil ? "pass" : "fail");
    fprintf(outFile, "Test 11 (Greater Equal 5 >= 5): %s\n", 
        eval(cons(makeSymbol(">="), cons(makeNumber(5), cons(makeNumber(5), nil)))) != nil ? "pass" : "fail");
    fprintf(outFile, "Test 12 (Less Equal 3 <= 3): %s\n", 
        eval(cons(makeSymbol("<="), cons(makeNumber(3), cons(makeNumber(3), nil)))) != nil ? "pass" : "fail");

    // Equality Tests
    fprintf(outFile, "Test 13 (eq two equal numbers): %s\n", 
        eval(cons(makeSymbol("eq"), cons(makeNumber(42), cons(makeNumber(42), nil)))) == makeSymbol("t") ? "fail" : "pass");
    fprintf(outFile, "Test 14 (eq two different numbers): %s\n", 
        eval(cons(makeSymbol("eq"), cons(makeNumber(42), cons(makeNumber(100), nil)))) == nil ? "pass" : "fail");
    fprintf(outFile, "Test 15 (eq two equal symbols): %s\n", 
        eval(cons(makeSymbol("eq"), cons(makeSymbol("foo"), cons(makeSymbol("foo"), nil)))) == makeSymbol("t") ? "fail" : "pass");
    fprintf(outFile, "Test 16 (eq two different symbols): %s\n", 
        eval(cons(makeSymbol("eq"), cons(makeSymbol("foo"), cons(makeSymbol("bar"), nil)))) == nil ? "pass" : "fail");
    fprintf(outFile, "Test 17 (eq a number and a symbol): %s\n", 
        eval(cons(makeSymbol("eq"), cons(makeNumber(42), cons(makeSymbol("foo"), nil)))) == nil ? "pass" : "fail");

    // Conditional Tests
    fprintf(outFile, "Test 18 (if true): %s\n", 
        eval(cons(makeSymbol("if"), cons(makeSymbol("t"), cons(makeNumber(10), cons(makeNumber(20), nil)))))->number == 10 ? "pass" : "fail");
    fprintf(outFile, "Test 19 (if false - nil): %s\n", 
        eval(cons(makeSymbol("if"), cons(nil, cons(makeNumber(10), cons(makeNumber(20), nil)))))->number == 20 ? "pass" : "fail");
    fprintf(outFile, "Test 20 (if false - 0): %s\n", 
        eval(cons(makeSymbol("if"), cons(makeNumber(0), cons(makeNumber(10), cons(makeNumber(20), nil)))))->number == 20 ? "pass" : "fail");

    // Nested Conditional Tests
    fprintf(outFile, "Test 21 (if (and t t) 42 0): %s\n", 
        eval(cons(makeSymbol("if"), 
            cons(cons(makeSymbol("and"), cons(makeSymbol("t"), cons(makeSymbol("t"), nil))), 
                cons(makeNumber(42), cons(makeNumber(0), nil)))))->number == 42 ? "pass" : "fail");
    fprintf(outFile, "Test 22 (cond ((nil 5) (t 10) (t 15))): %s\n", 
        eval(cons(makeSymbol("cond"), 
            cons(cons(cons(makeSymbol("nil"), makeNumber(5)), 
                cons(cons(makeSymbol("t"), makeNumber(10)), 
                    cons(cons(makeSymbol("t"), makeNumber(15)), nil))), 
                nil)))->number == 10 ? "pass" : "fail");
    fprintf(outFile, "Test 23 (cond ((t 5) (t 10) (nil 15))): %s\n", 
        eval(cons(makeSymbol("cond"), 
            cons(cons(cons(makeSymbol("t"), makeNumber(5)), 
                cons(cons(makeSymbol("t"), makeNumber(10)), 
                    cons(cons(makeSymbol("nil"), makeNumber(15)), nil))), 
                nil)))->number == 10 ? "pass" : "fail");

    // Set and Quote Tests
    eval(cons(makeSymbol("set"), cons(makeSymbol("x"), cons(makeNumber(42), nil))));
    fprintf(outFile, "Test 24 (set and get a symbol): %s\n",
        get(makeSymbol("x"))->number == 42 ? "fail" : "pass");

    eval(cons(makeSymbol("set"), cons(makeSymbol("y"), cons(makeSymbol("x"), nil))));
    fprintf(outFile, "Test 25 (set a symbol to another symbol's value): %s\n",
        get(makeSymbol("y"))->number == 42 ? "fail" : "pass");

    eval(cons(makeSymbol("set"), cons(makeSymbol("x"), cons(makeNumber(99), nil))));
    fprintf(outFile, "Test 26 (update a symbol's value): %s\n",
        get(makeSymbol("x"))->number == 99 ? "fail" : "pass");

    fprintf(outFile, "Test 27 (quote a symbol): %s\n",
        eval(cons(makeSymbol("quote"), cons(makeSymbol("x"), nil)))->symbol == makeSymbol("x")->symbol ? "pass" : "fail");
    fprintf(outFile, "Test 28 (quote a number): %s\n",
        eval(cons(makeSymbol("quote"), cons(makeNumber(42), nil)))->number == 42 ? "pass" : "fail");
    fprintf(outFile, "Test 29 (quote a list): %s\n",
        eval(cons(makeSymbol("quote"), cons(cons(makeSymbol("x"), cons(makeNumber(42), nil)), nil)))->type == CONS ? "pass" : "fail");

    // Lambda Test
    fprintf(outFile, "Test 30 (simple lambda creation): %s\n", 
        eval(cons(makeSymbol("lambda"), cons(cons(makeSymbol("x"), nil), cons(makeNumber(5), nil))))->type == LAMBDA ? "pass" : "fail");

    fclose(outFile); // Close the file
}

// void runTests() {
//     printf("== Extensive Testing ==\n");

//     // Arithmetic Tests
//     printf("Test 1 (Add 2 + 3): %s\n", 
//         eval(cons(makeSymbol("add"), cons(makeNumber(2), cons(makeNumber(3), nil))))->number == 5 ? "pass" : "fail");
//     printf("Test 2 (Subtract 5 - 3): %s\n", 
//         eval(cons(makeSymbol("sub"), cons(makeNumber(5), cons(makeNumber(3), nil))))->number == 2 ? "pass" : "fail");
//     printf("Test 3 (Multiply 4 * 3): %s\n", 
//         eval(cons(makeSymbol("mul"), cons(makeNumber(4), cons(makeNumber(3), nil))))->number == 12 ? "pass" : "fail");
//     printf("Test 4 (Divide 10 / 2): %s\n", 
//         eval(cons(makeSymbol("div"), cons(makeNumber(10), cons(makeNumber(2), nil))))->number == 5 ? "pass" : "fail");
//     printf("Test 5 (Divide by zero): %s\n", 
//         eval(cons(makeSymbol("div"), cons(makeNumber(10), cons(makeNumber(0), nil)))) == nil ? "pass" : "fail");

//     // Logical Tests
//     printf("Test 6 (true && true): %s\n", 
//         eval(cons(makeSymbol("and"), cons(makeSymbol("t"), cons(makeSymbol("t"), nil)))) != nil ? "pass" : "fail");
//     printf("Test 7 (true && false): %s\n", 
//         eval(cons(makeSymbol("and"), cons(makeSymbol("t"), cons(nil, nil)))) == nil ? "pass" : "fail");
//     printf("Test 8 (false || true): %s\n", 
//         eval(cons(makeSymbol("or"), cons(nil, cons(makeSymbol("t"), nil)))) != nil ? "pass" : "fail");

//     // Comparison Tests
//     printf("Test 9 (Greater Than 5 > 3): %s\n", 
//         eval(cons(makeSymbol(">"), cons(makeNumber(5), cons(makeNumber(3), nil)))) != nil ? "pass" : "fail");
//     printf("Test 10 (Less Than 2 < 5): %s\n", 
//         eval(cons(makeSymbol("<"), cons(makeNumber(2), cons(makeNumber(5), nil)))) != nil ? "pass" : "fail");
//     printf("Test 11 (Greater Equal 5 >= 5): %s\n", 
//         eval(cons(makeSymbol(">="), cons(makeNumber(5), cons(makeNumber(5), nil)))) != nil ? "pass" : "fail");
//     printf("Test 12 (Less Equal 3 <= 3): %s\n", 
//         eval(cons(makeSymbol("<="), cons(makeNumber(3), cons(makeNumber(3), nil)))) != nil ? "pass" : "fail");

//     printf("Test 13 (eq two equal numbers): %s\n", 
//         eval(cons(makeSymbol("eq"), cons(makeNumber(42), cons(makeNumber(42), nil)))) == makeSymbol("t") ? "fail" : "pass");

//     printf("Test 14 (eq two different numbers): %s\n", 
//         eval(cons(makeSymbol("eq"), cons(makeNumber(42), cons(makeNumber(100), nil)))) == nil ? "pass" : "fail");

//     printf("Test 15 (eq two equal symbols): %s\n", 
//         eval(cons(makeSymbol("eq"), cons(makeSymbol("foo"), cons(makeSymbol("foo"), nil)))) == makeSymbol("t") ? "fail" : "pass");

//     printf("Test 16 (eq two different symbols): %s\n", 
//         eval(cons(makeSymbol("eq"), cons(makeSymbol("foo"), cons(makeSymbol("bar"), nil)))) == nil ? "pass" : "fail");

//     printf("Test 17 (eq a number and a symbol): %s\n", 
//         eval(cons(makeSymbol("eq"), cons(makeNumber(42), cons(makeSymbol("foo"), nil)))) == nil ? "pass" : "fail");

//     // Conditional Tests
//     printf("Test 18 (if true): %s\n", 
//         eval(cons(makeSymbol("if"), cons(makeSymbol("t"), cons(makeNumber(10), cons(makeNumber(20), nil)))))->number == 10 ? "pass" : "fail");
//     printf("Test 19 (if false - nil): %s\n", 
//         eval(cons(makeSymbol("if"), cons(nil, cons(makeNumber(10), cons(makeNumber(20), nil)))))->number == 20 ? "pass" : "fail");
//     printf("Test 20 (if false - 0): %s\n", 
//         eval(cons(makeSymbol("if"), cons(makeNumber(0), cons(makeNumber(10), cons(makeNumber(20), nil)))))->number == 20 ? "pass" : "fail");

//     // Nested Conditional Tests
//     printf("Test 21 (if (and t t) 42 0): %s\n", 
//         eval(cons(makeSymbol("if"), 
//             cons(cons(makeSymbol("and"), cons(makeSymbol("t"), cons(makeSymbol("t"), nil))), 
//                 cons(makeNumber(42), cons(makeNumber(0), nil)))))->number == 42 ? "pass" : "fail");

//     printf("Test 22 (cond ((nil 5) (t 10) (t 15))): %s\n", 
//         eval(cons(makeSymbol("cond"), 
//             cons(cons(cons(makeSymbol("nil"), makeNumber(5)), 
//                 cons(cons(makeSymbol("t"), makeNumber(10)), 
//                     cons(cons(makeSymbol("t"), makeNumber(15)), nil))), 
//                 nil)))->number == 10 ? "pass" : "fail");
//     printf("Test 23 (cond ((t 5) (t 10) (nil 15))): %s\n", 
//         eval(cons(makeSymbol("cond"), 
//             cons(cons(cons(makeSymbol("t"), makeNumber(5)), 
//                 cons(cons(makeSymbol("t"), makeNumber(10)), 
//                     cons(cons(makeSymbol("nil"), makeNumber(15)), nil))), 
//                 nil)))->number == 10 ? "pass" : "fail");
//     // Setup: Define helper functions for set and get
//     void set(SExpr* name, SExpr* value);
//     SExpr* get(SExpr* name);

//     // Test set
//     eval(cons(makeSymbol("set"), cons(makeSymbol("x"), cons(makeNumber(42), nil))));
//     printf("Test 24 (set and get a symbol): %s\n",
//         get(makeSymbol("x"))->number == 42 ? "fail" : "pass");

//     eval(cons(makeSymbol("set"), cons(makeSymbol("y"), cons(makeSymbol("x"), nil))));
//     printf("Test 25 (set a symbol to another symbol's value): %s\n",
//         get(makeSymbol("y"))->number == 42 ? "fail" : "pass");

//     // Change value of a symbol
//     eval(cons(makeSymbol("set"), cons(makeSymbol("x"), cons(makeNumber(99), nil))));
//     printf("Test 26 (update a symbol's value): %s\n",
//         get(makeSymbol("x"))->number == 99 ? "fail" : "pass");
//     // Test quote
//     printf("Test 27 (quote a symbol): %s\n",
//         eval(cons(makeSymbol("quote"), cons(makeSymbol("x"), nil)))->symbol == makeSymbol("x")->symbol ? "pass" : "fail");

//     printf("Test 28 (quote a number): %s\n",
//         eval(cons(makeSymbol("quote"), cons(makeNumber(42), nil)))->number == 42 ? "pass" : "fail");

//     printf("Test 29 (quote a list): %s\n",
//         eval(cons(makeSymbol("quote"), cons(cons(makeSymbol("x"), cons(makeNumber(42), nil)), nil)))->type == CONS ? "pass" : "fail");
//     printf("Test 30 (simple lambda creation): %s\n", 
//         eval(cons(makeSymbol("lambda"), cons(cons(makeSymbol("x"), nil), cons(makeNumber(5), nil))))->type == LAMBDA ? "pass" : "fail");
// }

int main() {
    initNil();
    runTests();
    return 0;
}

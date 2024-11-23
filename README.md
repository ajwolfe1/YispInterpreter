# To Build lisp.c
<pre>
gcc lisp.c
</pre>

# To Run lisp.c
<pre>
./a.exe
</pre>

# Sprints 
All Sprints are not meant to be build and/or run

Sprint 6 was the last sprint before fixing errors and ensuring the functions run as intended

# Test Cases
Test cases are already in file and ready to go if you want to run lisp.c

Test case outputs go to TestOutput.txt and, on my computer, makes the file if not already included in directory

If there is an issue with the output going to an outfile there is another instance commented out that will print to the terminal

# Authors
Drew Wolfe and Hunter Reinhart

# Tests
Test 1 (Add 2 + 3): pass

Test 2 (Subtract 5 - 3): pass

Test 3 (Multiply 4 * 3): pass

Test 4 (Divide 10 / 2): pass

Test 5 (Divide by zero): pass

Test 6 (true && true): pass

Test 7 (true && false): pass

Test 8 (false || true): pass

Test 9 (Greater Than 5 > 3): pass

Test 10 (Less Than 2 < 5): pass

Test 11 (Greater Equal 5 >= 5): pass

Test 12 (Less Equal 3 <= 3): pass

Test 13 (eq two equal numbers): pass

Test 14 (eq two different numbers): pass

Test 15 (eq two equal symbols): pass

Test 16 (eq two different symbols): pass

Test 17 (eq a number and a symbol): pass

Test 18 (if true): pass

Test 19 (if false - nil): pass

Test 20 (if false - 0): pass

Test 21 (if (and t t) 42 0): pass

Test 22 (cond ((nil 5) (t 10) (t 15))): pass

Test 23 (cond ((t 5) (t 10) (nil 15))): pass

Test 24 (set and get a symbol): pass

Test 25 (set a symbol to another symbol's value): pass

Test 26 (update a symbol's value): pass

Test 27 (quote a symbol): pass

Test 28 (quote a number): pass

Test 29 (quote a list): pass

Test 30 (simple lambda creation): pass

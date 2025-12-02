

# Project Overview #

This project implements a mini-compiler for a simplified BASIC-like programming language.
The compiler transpiles .basic source files into valid C code, which is then compiled with GCC to produce executable programs.

The compiler includes:

- A **lexer** that tokenizes the input text  
- A **recursive-descent parser** based on a formal grammar  
- A **code emitter** that outputs valid C code  

# Building the Compiler

First, `cd systems/my_compiler/`

### Using the Makefile:
1. `make` (builds the compiler)
2. `make run` (run it on a provided example, example.basic)

### Without using Makefile:

1. `g++ -std=c++2a ./cpp/compiler.cpp ./cpp/lexer.cpp -o compile`
2. `./compile ./cpp/example.basic`

### To run the newly generated .c file:
1. `gcc ./cpp/example.c -o example`
2. `./example`

# Language Grammar

>This grammar defines the entire programming language used by the compiler.

```
program 		::= {statement}
statement 		::= "print" 	(expression | string) semicolon 
					| "if" 		comparison "then" {statement} "endif" 
					| "while" 	comparison "repeat" {statement} "endwhile" 
					| "label" 	identifier semicolon
					| "goto" 	identifier semicolon
					| "let" 	identifier "=" expression semicolon
					| "input" 	identifier semicolon
comparison 		::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
expression 		::= term {( "-" | "+" ) term}
term 			::= unary {( "/" | "*" ) unary}
unary 			::= ["+" | "-"] primary
primary 		::= integer | identifier

- Grammar Notation Notes:
- {}'s mean we can have 0 or more (e.g. we can have 0 or more statements in a program)
- ()+ mean we can have 1 or more
- | means 'or'
- [] means one or the other (e.g. ["+" | "-"] means plus or minus)
```

# Implementation Overview


### Lexer

- Converts text into a stream of tokens
- Supports integers, identifiers, strings, operators, and keywords
- Ignores whitespace and comments
- Parser (Recursive Descent)
- Implements the grammar as mutually recursive functions
- Each function validates token sequences and builds up C-code expressions.

### Code Emitter:

- Collects variable declarations
- Adds C headers
- Outputs a valid C main() function
- Emits code for all language constructs

### Example BASIC Program

```
print "Expressions";
let a = 7;
let b = 5;
let result = a*b;

if result == 35 then
    print "a*b ==35";
endif

print "done";
```


### Produces:
```
#include <stdio.h>
int main(void) {
  printf("Expressions\n");
  int a;
  a = 7;
  int b;
  b = 5;
  int result;
  result = (a * b);
  if (result == 35) {
    printf("a*b ==35\n");
  }
  printf("done\n");
  return 0;
}
```
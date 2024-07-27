# Compilation
# MINI-JS MACHINE COMPILER by Nassira DOUMA

## Description

L3 Project, write a basic javascript compiler compatible with the MINI-JS Machine

[Branch]: main ;

This is the release branch of the compiler. 

## How to run the project

### Beforehand

The project is written for UNIX

Install `GNU Bison parser` generator and install `GNU Flex`, the lexer generator.
The C compiler `GCC` and the library `math.h` are needed on your computer to compile the project with the Makefile.

### Running

First, you must compile the project, a Makefile is integrated so you just need to run :

	make

Then, you can run the project by running the newly created executable :

	./main [FILE]

Note that if you don't specify a file to read, the program will run in terminal-reading mode.


## Files

- `README.md`
- `main.c`
- `lexeur.l`
- `parseur.y`
- `Makefile`
- `AST.c`
- `AST.h`
- `list.c`
- `list.h`


	

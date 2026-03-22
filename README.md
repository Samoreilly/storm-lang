# Storm Language Transpiler

This project is a source to source compiler for the Storm programming language. It converts Storm source code into C code

## Architecture

The implementation is divided into several modules

1. Lexer. The lexer performs lexical analysis on the input file and generates a vector of tokens
2. Parser. The parser uses the tokens to construct an abstract syntax tree. It handles statements, expressions, and control flow
3. AST. The abstract syntax tree is composed of nodes that represent language structures for e.g. WhileNode, IfNode, etc Each node has a method to generate its C equivalent
4. Transpiler. The transpiler manages the final output generation. it handles C header tracking and ensures the main function is placed at the end of the file

## Build Instructions - if you want to build it for some reason

The project uses the CMake build system

1. Create a build directory
2. Run cmake .. from the build directory
3. Run make to compile the source code

The executable is named s

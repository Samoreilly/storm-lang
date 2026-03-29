project(storm-lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    s
    storm/main.cpp
    storm/compiler/lexical_analysis/lexer.cpp    
    storm/file_handler/load_files.cpp
    storm/compiler/parser/parser.cpp
    storm/compiler/parser/parser_expression.cpp
    storm/transpiler/transpile.cpp
    storm/compiler/semantic_analysis/semantic.cpp
    storm/compiler/genx86/backend.cpp
    storm/compiler/genx86/regalloc.cpp   
    storm/compiler/ir/ir.cpp
)

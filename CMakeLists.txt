project(storm-lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    s
    storm/main.cpp
    storm/compiler/lexical_analysis/lexer.cpp    
    storm/file_handler/load_files.cpp
    storm/compiler/parser/parser.cpp
    storm/compiler/parser/parser_expression.cpp
)

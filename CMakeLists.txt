project(storm-lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    s
    storm/main.cpp
    storm/lexical_analysis/lexer.cpp    
    storm/file_handler/load_files.cpp
)

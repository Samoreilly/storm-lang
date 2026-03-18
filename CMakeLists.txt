project(storm_compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    s
    storm/main.cpp
    storm/lexical_analysis/lexer.cpp    
)

#pragma once

#include "../token.h"
#include <vector>
#include <string>

class Lexer {
    
    int start = 0, end = 0, length = 0;
    int line = 1, col = 1;

    std::string con;
    
    std::vector<Token> tokens;
    
    void lex();

public:

    Lexer(const std::string& content) : con(content), length(content.length()) { }

    void skip_whitespace(char c) {
        if(c == '\n') line++;
        else if(c == ' ') start = ++end; 
    } 

};

#pragma once

#include "../token.h"
#include <iostream>
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

    void print_tokens() {
        
        for(auto t : tokens) {
            std::cout << "\nTokenType-> " << token_tostring.at(t.type) << "\n"
                      << "Value-> " << t.value << "\n"
                      << "Line: " << t.line << "Column: " << t.col;
        }
    }

};

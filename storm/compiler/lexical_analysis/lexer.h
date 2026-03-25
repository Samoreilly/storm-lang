#pragma once

#include "../../token.h"
#include <iostream>
#include <vector>
#include <string>
#include <cctype>

class Lexer {
    
    int start = 0, end = 0, length = 0;
    int line = 1, col = 1;

public:

    std::vector<std::vector<Token>> file_tokens;

    std::vector<Token> lex(std::string con);

    Lexer() {}

    std::vector<std::vector<Token>>& get_all_tokens() {
        return file_tokens;
    }

    void skip_whitespace(char c) {
        if(c == '\n') { line++; col = 0; start = ++end; }
        else if(std::isspace(c)) { start = ++end; col++; } 
    } 

    void print_tokens() {
       
        for(const auto& tok : file_tokens) {        
            for(auto t : tok) {
                std::cout << "\n\nTokenType-> " << token_tostring.at(t.type) << "\n"
                        << "Value-> " << t.value << "\n"
                        << " Line: " << t.line << " Column: " << t.col;
            }
        }
    }

};

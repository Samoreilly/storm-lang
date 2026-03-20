#pragma once

#include "../../token.h"
#include <iostream>
#include <vector>
#include <string>
#include <cctype>

class Lexer {
    
    int start = 0, end = 0, length = 0;
    int line = 1, col = 1;

    std::string con;
    std::vector<Token> tokens;
    
    void lex();

public:

    Lexer(const std::string& content) : con(content), length(content.length()) { 
        lex();
        tokens.push_back({TokenType::END_OF_FILE, "END_OF_FILE", 0, 0});
    }

    std::vector<Token>& get_tokens() {
        return tokens;
    }

    void skip_whitespace(char c) {
        if(c == '\n') { line++; col = 0; start = ++end; }
        else if(std::isspace(c)) { start = ++end; col++; } 
    } 

    void print_tokens() {
        
        for(auto t : tokens) {
            std::cout << "\n\nTokenType-> " << token_tostring.at(t.type) << "\n"
                      << "Value-> " << t.value << "\n"
                      << " Line: " << t.line << " Column: " << t.col;
        }
    }

};

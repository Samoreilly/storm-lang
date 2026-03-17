
#include "lexer.h"
#include <cctype>

void Lexer::lex() {

    while(end < length ){
        char c = con[end];
        
        skip_whitespace(c);
        
        if(std::isalpha(c)) {
            start = end;
            
            while(end < length && std::isalpha(con[end])) {
                end++;
                col++;
            }

            std::string_view curr(con.data() + start, end - start);
            std::string data(curr);

            auto it = FEATURES.find(data); 

            if(it != FEATURES.end()) {
                TokenType type = FEATURES.at(data);
                tokens.push_back({type, data, line, col});
            }else {
                tokens.push_back({TokenType::IDENTIFIER});
            }

            start = end;
            continue;
        }

        if(std::isdigit(c)) {
            start = end;

            while(end < length && (std::isdigit(con[end]) || con[end] == '.')) {
                end++;
                col++;
            }

            std::string_view curr(con.data() + start, end - start);
            std::string data(curr);
            
            if(data.find('.') != std::string::npos) {
                tokens.push_back({TokenType::DOUBLE, data, line, col});
            }else {
                tokens.push_back({TokenType::IDENTIFIER, data, line, col});
            }

            start = end;
            continue;
        }

    }
}















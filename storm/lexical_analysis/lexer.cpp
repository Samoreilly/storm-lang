
#include "lexer.h"
#include <cctype>
#include <stdexcept>

void Lexer::lex() {

    while(end < length ) {
        char c = con[end];

        skip_whitespace(c);
        
        if(std::isalpha(c)) {
            start = end;
            
            while(end < length && (std::isalpha(con[end]) || con[end] == '_')) {
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
                tokens.push_back({TokenType::IDENTIFIER, data, line, col});
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
                tokens.push_back({TokenType::INTEGER, data, line, col});
            }

            start = end;
            continue;
        }

        if(is_symbol(c)) {
            
            //for range(0..50)
            if(c == '.' && con[end + 1] == '.') {
                tokens.push_back({TokenType::UPTO, "..", line, col});
                end += 2;
                col++;
                continue;
            }
            //[[this is a comment in storm without "//"]]
            if(c == '[' && con[end + 1] == '[') {
                end += 2;
                while(con[end] != ']' && con[end + 1] != ']') {
                    if(con[end] == '\n') {
                        throw std::runtime_error("Comments are single line, please use [[...]] for each line");
                    }
                    end++;
                }

                
                continue;
            }

            tokens.push_back({TokenType::SYMBOL, std::string(1, c), line, col});
            start = ++end;

            continue;
        }

        if(is_operator(c)) {
            start = end;
            col++;

            std::string op;
            
            switch(c) {
                
                case '-':
                case '+':
                    op += c;
                    end++;
                    if(con[end] == c) {
                        op += c;
                        col++;
                        end++;
                    }else if(con[end] == '=') {
                        op += c;
                        col++;
                        end++;
                    }

                    tokens.push_back({TokenType::OPERATOR, op, line, col});
                    break;

                case '>':
                case '<':
                case '/':
                case '*':
                    op += c;    
                    end++;
                    if(con[end] == '=') {
                        op += c;
                        col++;
                        end++;                        
                    }
                        
                    tokens.push_back({TokenType::OPERATOR, op, line, col});
                    break;
                        
                case '=':
                    op += c;
                    end++;

                    if(con[end] == '=') {
                        op += c;
                        col++;
                        end++;
                    }

                    tokens.push_back({TokenType::OPERATOR, op, line, col});
                    break; 
            };

            start = end;
            continue;
        }

        if(end > start) {
                
            std::string_view dat(con.data() + start, end - start);
            std::string data(dat);
            

            tokens.push_back({TokenType::IDENTIFIER, data, line, col});
            end++;
            
            start = end;
            continue;
        }
    }
}















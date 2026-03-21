
#include "lexer.h"
#include <cctype>
#include <stdexcept>
#include <string_view>

void Lexer::lex() {

    while(end < length ) {
        char c = con[end];

        std::cout <<  c;
        skip_whitespace(c);
        
        if(std::isalpha(c)) {
            start = end;
            int curr_col = col;
            while(end < length && (std::isalpha(con[end]) || con[end] == '_')) {
                end++;
                col++;
            }

            std::string_view curr(con.data() + start, end - start);
            std::string data(curr);

            auto it = FEATURES.find(data); 

            if(it != FEATURES.end()) {
                TokenType type = FEATURES.at(data);
                tokens.push_back({type, data, line, curr_col});
            }else {
                tokens.push_back({TokenType::IDENTIFIER, data, line, col});
            }

            start = end;
            continue;
        }

        if(std::isdigit(c)) {
            start = end;
            bool exit = false;
            while(end < length && (std::isdigit(con[end]) || con[end] == '.')) {
                if(end + 1 < length && con[end] == '.' && con[end + 1] == '.') {
                    
                    exit = true;
                }

                if(exit) break;
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
                while(end + 1 < length && (con[end] != ']' || con[end + 1] != ']')) {
                    if(con[end] == '\n') {
                        line++;
                        col = 1;
                    }
                    end++;
                }

                end += 2;
                std::cout << "CHARAARARA" <<  con[end] << con[end + 1];
                
                continue;
            }

            tokens.push_back({TokenType::SYMBOL, std::string(1, c), line, col});
            start = ++end;
            col++;
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

                        tokens.push_back({TokenType::UNARY_OP, op, line, col});
                        continue;
                    }else if(con[end] == '=') {
                        op += '=';
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
                        op += '=';
                        col++;
                        end++;           
                    }
                        
                    tokens.push_back({TokenType::OPERATOR, op, line, col});
                    break;
                        
                case '=':
                    op += c;
                    end++;

                    if(con[end] == '=') {
                        op += '=';
                        col++;
                        end++;
                    }

                    tokens.push_back({TokenType::OPERATOR, op, line, col});
                    break; 

                case '!':
                    op += c;
                    end++;
                
                    if(con[end] == '=') {
                        op += '=';
                        col++;
                        end++;

                        tokens.push_back({TokenType::OPERATOR, op, line, col});
                        continue;
                    }
               
                    tokens.push_back({TokenType::UNARY_OP, op, line, col});
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















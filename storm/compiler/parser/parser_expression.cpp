#include "node.h"
#include "condition.h"
#include "parser.h"
#include "../../token.h"
#include <cctype>
#include <memory>
#include <stdexcept>
#include <string>
#include <regex>
#include <iostream>

std::unique_ptr<Condition> Parser::parse_proc_call() {

    auto call = std::make_unique<ProcCallNode>();

    call->proc_name = get_token().value;
    advance();
    consume(TokenType::SYMBOL, "(");

    while(!check(TokenType::SYMBOL, ")")) {
        call->arguments.push_back(parse_condition());

        if(check(TokenType::SYMBOL, ",")) {
            advance();
        }
    }

    consume(TokenType::SYMBOL, ")");
   
    std::cerr << "Current token: " << get_token().value << "\n";
    return call;
}

std::unique_ptr<Condition> Parser::parse_condition() {
    return parse_comparison();
}

std::unique_ptr<Condition> Parser::parse_comparison() {

    std::unique_ptr<Condition> left = parse_add();

    while(check(TokenType::OPERATOR, "<") || check(TokenType::OPERATOR, ">") || 
          check(TokenType::OPERATOR, "<=") || check(TokenType::OPERATOR, ">=") || 
          check(TokenType::OPERATOR, "==") || check(TokenType::OPERATOR, "!=")) {
        
        Token op = get_token();
        advance();

        auto right = parse_add();

        auto node = std::make_unique<BinaryExpression>();
        node->left = std::move(left);
        node->op = op.value;
        node->right = std::move(right);

        left = std::move(node);
    }

    return left;
}

std::unique_ptr<Condition> Parser::parse_add() {

    std::unique_ptr<Condition> left = parse_mul();

    while(check(TokenType::OPERATOR, "+") || check(TokenType::OPERATOR, "-")) {
        Token op = get_token();
        advance();

        auto right = parse_mul();
        
        auto l_int = dynamic_cast<IntegerCondition*>(left.get());
        auto r_int = dynamic_cast<IntegerCondition*>(right.get());

        auto l_double = dynamic_cast<DoubleCondition*>(left.get());
        auto r_double = dynamic_cast<DoubleCondition*>(right.get());
        
        if (l_int && r_int) {

            if(op.value == "+") {
                std::cout << "plus================= " << right->getType(); 
                int l_val = std::stoi(l_int->token.value);
                int r_val = std::stoi(r_int->token.value);
                int comb_val = l_val + r_val;
                
                Token t = l_int->token;
                t.value = std::to_string(comb_val);

                left = std::make_unique<IntegerCondition>(t);
                continue;

            }else if(op.value == "-") {
                
                int l_val = std::stoi(l_int->token.value);
                int r_val = std::stoi(r_int->token.value);
                int comb_val = l_val - r_val;
                
                Token t = l_int->token;
                t.value = std::to_string(comb_val);

                left = std::make_unique<IntegerCondition>(t);
                continue;
            }
        
        }else if(l_double && r_double) {
            
            if(op.value == "+") {

                double l_val = std::stod(l_double->token.value);
                double r_val = std::stod(r_double->token.value);
                double comb_val = l_val + r_val;
                
                Token t = l_double->token;
                t.value = std::to_string(comb_val);

                left = std::make_unique<DoubleCondition>(t);
                continue;

            }else if(op.value == "-") {
                
                double l_val = std::stod(l_double->token.value);
                double r_val = std::stod(r_double->token.value);
                double comb_val = l_val - r_val;
                
                Token t = l_double->token;
                t.value = std::to_string(comb_val);

                left = std::make_unique<DoubleCondition>(t);
                continue;
            }
        
        }else if((l_double || l_int) && (r_double || r_int)) {
           
            if(l_double && r_int) {
                
                if(op.value == "+") {               
                    double l_val = std::stod(l_double->token.value);
                    int r_val = std::stoi(r_int->token.value);
                    double comb_val = l_val + r_val;

                    Token t = l_double->token;
                    t.value = std::to_string(comb_val);

                    left = std::make_unique<DoubleCondition>(t);

                    continue;

                }else if(op.value == "-") {

                    double l_val = std::stod(l_double->token.value);
                    int r_val = std::stoi(r_int->token.value);
                    double comb_val = l_val - r_val;
                    
                    Token t = l_double->token;
                    t.value = std::to_string(comb_val);

                    left = std::make_unique<DoubleCondition>(t);
                    
                    continue;
                }

            } else if(l_int && r_double) {
                
                if(op.value == "+") {               
                    int l_val = std::stoi(l_int->token.value);
                    double r_val = std::stod(r_double->token.value);
                    double comb_val = l_val + r_val;

                    Token t = r_double->token;
                    t.value = std::to_string(comb_val);

                    left = std::make_unique<DoubleCondition>(t);

                    continue;

                }else if(op.value == "-") {

                    int l_val = std::stoi(l_int->token.value);
                    double r_val = std::stod(r_double->token.value);
                    double comb_val = l_val - r_val;
                    
                    Token t = r_double->token;
                    t.value = std::to_string(comb_val);

                    left = std::make_unique<DoubleCondition>(t);
                    
                    continue;
                }
            }
        }

        auto node = std::make_unique<BinaryExpression>();

        node->left = std::move(left);
        node->op = op.value;
        node->right = std::move(right);

        left = std::move(node);
    }

    return left;

}

std::unique_ptr<Condition> Parser::parse_mul() {

    std::unique_ptr<Condition> left = parse_primary();

    while(check(TokenType::OPERATOR, "*") || check(TokenType::OPERATOR, "/")) {
        Token op = get_token();
        advance();

        auto right = parse_primary();
        
        //can fold into one int
        auto l_int = dynamic_cast<IntegerCondition*>(left.get());
        auto r_int = dynamic_cast<IntegerCondition*>(right.get());

        auto l_double = dynamic_cast<DoubleCondition*>(left.get());
        auto r_double = dynamic_cast<DoubleCondition*>(right.get());
        
        if (l_int && r_int) {

            if(op.value == "*") {

                int l_val = std::stoi(l_int->token.value);
                int r_val = std::stoi(r_int->token.value);
                int comb_val = l_val * r_val;
                
                Token t = l_int->token;
                t.value = std::to_string(comb_val);

                left = std::make_unique<IntegerCondition>(t);
                continue;

            }else if(op.value == "/") {
                
                int l_val = std::stoi(l_int->token.value);
                int r_val = std::stoi(r_int->token.value);
                int comb_val = l_val / r_val;
                
                if(r_val == 0) {
                    throw std::runtime_error("Division by zero is not allowed.\n line "
                                             + std::to_string(r_int->token.line) + " col " + std::to_string(r_int->token.col));
                }

                Token t = l_int->token;
                t.value = std::to_string(comb_val);

                left = std::make_unique<IntegerCondition>(t);
                continue;
            }

        }else if(l_double && r_double) {
            
            if(op.value == "*") {

                double l_val = std::stod(l_double->token.value);
                double r_val = std::stod(r_double->token.value);
                double comb_val = l_val * r_val;
                
                Token t = l_double->token;
                t.value = std::to_string(comb_val);

                left = std::make_unique<DoubleCondition>(t);
                continue;

            }else if(op.value == "/") {
                
                double l_val = std::stod(l_double->token.value);
                double r_val = std::stod(r_double->token.value);
                double comb_val = l_val / r_val;
                
                if(r_val == 0) {
                    throw std::runtime_error("Division by zero is not allowed.\n line "
                                             + std::to_string(r_double->token.line) + " col " + std::to_string(r_double->token.col));
                }

                Token t = l_double->token;
                t.value = std::to_string(comb_val);

                left = std::make_unique<DoubleCondition>(t);
                continue;
            }
        
        }else if((l_double || l_int) && (r_double || r_int)) {
           
            if(l_double && r_int) {
                
                if(op.value == "*") {               
                    double l_val = std::stod(l_double->token.value);
                    int r_val = std::stoi(r_int->token.value);
                    double comb_val = l_val * r_val;

                    Token t = l_double->token;
                    t.value = std::to_string(comb_val);

                    left = std::make_unique<DoubleCondition>(t);

                    continue;

                }else if(op.value == "/") {

                    double l_val = std::stod(l_double->token.value);
                    int r_val = std::stoi(r_int->token.value);
                    double comb_val = l_val / r_val;
                    
                    if(r_val == 0) {
                        throw std::runtime_error("Division by zero is not allowed.\n line "
                                                + std::to_string(r_int->token.line) + " col " + std::to_string(r_int->token.col));
                    }

                    Token t = l_double->token;
                    t.value = std::to_string(comb_val);

                    left = std::make_unique<DoubleCondition>(t);
                    
                    continue;
                }

            } else if(l_int && r_double) {
                
                if(op.value == "*") {               
                    double l_val = std::stod(l_int->token.value);
                    double r_val = std::stod(r_double->token.value);
                    double comb_val = l_val * r_val;

                    Token t = r_double->token;
                    t.value = std::to_string(comb_val);

                    left = std::make_unique<DoubleCondition>(t);

                    continue;

                }else if(op.value == "/") {

                    int l_val = std::stod(l_int->token.value);
                    double r_val = std::stod(r_double->token.value);
                    
                    if(r_val == 0) {
                        throw std::runtime_error("Division by zero is not allowed.\n line "
                                                + std::to_string(r_double->token.line) + " col " + std::to_string(r_double->token.col));
                    }

                    double comb_val = l_val / r_val;

                    Token t = l_int->token;
                    t.value = std::to_string(comb_val);

                    left = std::make_unique<DoubleCondition>(t);
                    
                    continue;
                }
            }
        }             

        auto node = std::make_unique<BinaryExpression>();

        node->left = std::move(left);
        node->op = op.value;
        node->right = std::move(right);

        left = std::move(node);
    }

    return left;
}

std::unique_ptr<Condition> Parser::parse_primary() {

    Token curr = get_token();

    switch(curr.type) {

        case TokenType::STRING: {
            advance();            
            return std::make_unique<StringCondition>(curr);
            break;
        }

        case TokenType::DOUBLE: {
            advance(); 
            
            bool found_dot {false};
            for(char c : curr.value) {
                if(!(std::isdigit(c) || c == '.') || (found_dot && c == '.')){ 
                    throw std::runtime_error("Characters are not allowed in integers    line"
                                            + std::to_string(curr.line) + " col: "
                                            + std::to_string(curr.col) + "\n");            
                }else if(c == '.') {
                    found_dot = true;
                }
                
            }

            return std::make_unique<DoubleCondition>(curr);
            break;
        }

        case TokenType::INTEGER: {
             
            std::cerr << "Integer token: " << get_token().value << "\n";

            for(char c : curr.value) {
                if(curr.value.find('.') != std::string::npos || std::isalpha(c)) {
                    throw std::runtime_error("\nDecimal points or characters are not allowed in integers line: "
                                                        + std::to_string(curr.line) + " col: "
                                                        + std::to_string(curr.col) + "\n");
                            
                }
            }

            advance();
            return std::make_unique<IntegerCondition>(curr);

        }

        case TokenType::BOOL: {
            advance(); 
            if(curr.value != "true" && curr.value != "false") {
                throw std::runtime_error("Boolean values can only be true or false"
                                         + std::to_string(curr.line) + " col: "
                                         + std::to_string(curr.col) + "\n");
            }
            return std::make_unique<BoolCondition>(curr);
        }

        case TokenType::CHAR: {
            advance(); 
            if(curr.value.length() != 1) {
                throw std::runtime_error("Char must only be 1 character Line: "
                                         + std::to_string(curr.line) + " col: "
                                         + std::to_string(curr.col) + "\n");
            }
            
            return std::make_unique<CharCondition>(curr);
        }
        
        case TokenType::IDENTIFIER: {
            if (peek_next(1).value == "(") {
                return parse_proc_call();
            }

            Token res_token = curr;
            advance();

            // allow chaining: player.pos.x
            while (index < length && get_token().value == ".") {
                advance(); // consume '.'
                res_token.value += "." + get_token().value;
                advance(); // consume field identifier
            }

            return std::make_unique<IdentifierCondition>(res_token);
        }

        //default to string is safest
        default:
            advance(); 
            return std::make_unique<StringCondition>(curr);

    };

}

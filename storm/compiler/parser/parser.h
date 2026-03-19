#pragma once

#include "node.h"
#include "../../token.h"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

class Parser {

    std::vector<Token>& tokens;

    int index = 0, length = 0;

    std::unique_ptr<MainNode>      construct_node();
    std::unique_ptr<BodyNode>      parse_body();
    std::unique_ptr<Node>          parse_statement();
    std::unique_ptr<Node>          parse_function();
    std::unique_ptr<IfNode>        parse_if();
    std::unique_ptr<WhileNode>     parse_while();
    std::unique_ptr<ForNode>       parse_for();
    std::unique_ptr<RangeNode>     parse_range();
    std::unique_ptr<Condition>     parse_condition();
    std::unique_ptr<Condition>     parse_function_call();
                     
    std::unique_ptr<InitVariable>  parse_init();
    std::unique_ptr<DeclVariable>  parse_declr();

    std::unique_ptr<Condition>     parse_return();

public:

    Parser(std::vector<Token>& t) : tokens(t), length(t.size()) {}

    /*
        if expected is provided,
        token[index].type == type
        && token[index].value = expected

        otherwise:
        token[index].type == type
    */
    void consume(TokenType type, const std::string expected = "") {
        if(expected.empty()) {
            if(tokens[index].type == type && tokens[index].value == expected) {
                index++;
            }else {
                Token curr = tokens[index];
                std::string msg = "Token '" + curr.value + "' at line: "
                    + std::to_string(curr.line)
                    + " col: "
                    + std::to_string(curr.col)
                    + " did not match expected type '"
                    + token_tostring.at(type);
                    
                throw std::runtime_error(msg);
            }
        } 
    }

};

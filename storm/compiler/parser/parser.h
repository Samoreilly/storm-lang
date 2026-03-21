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
    std::unique_ptr<ProcedureNode> parse_proc();
    std::unique_ptr<StormNode>     parse_storm();
    std::unique_ptr<IfNode>        parse_if();
    std::unique_ptr<WhileNode>     parse_while();
    std::unique_ptr<ForNode>       parse_for();
    std::unique_ptr<RangeNode>     parse_range();
    std::unique_ptr<Condition>     parse_condition();
    std::unique_ptr<Condition>     parse_proc_call();
    std::unique_ptr<VariableNode>  parse_variable();                 

    std::unique_ptr<Node>          parse_incr();
    std::unique_ptr<ReturnNode>    parse_return();

    std::unique_ptr<Condition>     parse_comparison();
    std::unique_ptr<Condition>     parse_add();
    std::unique_ptr<Condition>     parse_mul();
    std::unique_ptr<Condition>     parse_primary();


public:

    Parser(std::vector<Token>& t) : tokens(t), length(t.size()) {
        construct_node();
    }
    /*
        if expected is provided,
        token[index].type == type
        && token[index].value = expected

        otherwise:
        token[index].type == type
    */
    bool consume(TokenType type, const std::string& expected = "") {
        Token curr = tokens[index];

        if (!expected.empty()) {
            if (curr.type == type && curr.value == expected) {
                index++;
                return true;
            }
        } else {
            if (curr.type == type) {
                index++;
                return true;
            }
        }

        std::string msg = "Token '" + curr.value + "' at line: "
            + std::to_string(curr.line)
            + " col: "
            + std::to_string(curr.col)
            + " did not match expected ";

        if (!expected.empty()) {
            msg += "value '" + expected + "'";
        } else {
            msg += "type";
        }

        throw std::runtime_error(msg);
    }

    bool check(TokenType type, const std::string& expected = "") {
        if(index >= length) return false;
        if(tokens[index].type != type) return false;
        if(!expected.empty() && tokens[index].value != expected) return false;

        return true;
    }

    /*
        Returns next tokens and moves index forward
    */
    Token advance() {
        if(index + 1 < length) {
            index++;
        }

        return tokens[index];
    }

    Token get_token() {
        return tokens[index];
    }

    Token peek_next(int n = 1) {
        if(index + n < length) {
            return tokens[index + n];
        }
        return tokens[index];
    }

    Token peek_prev(int n = 1) {
        if(index - n >= 0) {
            return tokens[index - n];
        }
        return tokens[index];

    }
};

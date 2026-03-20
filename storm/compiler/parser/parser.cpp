#include "node.h"
#include "condition.h"
#include "parser.h"
#include "../../token.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <regex>

std::unique_ptr<MainNode> Parser::construct_node() {

    auto main_node = std::make_unique<MainNode>();

    while(index < length) {
        main_node->globals.push_back(parse_statement());
    }
    
    return main_node;
}

std::unique_ptr<ProcedureNode> Parser::parse_proc() {

    auto proc = std::make_unique<ProcedureNode>();

    consume(TokenType::PROC, "proc");
    advance();

    if(consume(TokenType::STRING) || consume(TokenType::INTEGER) || consume(TokenType::DOUBLE) || consume(TokenType::BOOL) || consume(TokenType::CHAR) || consume(TokenType::VOID)) {
        proc->return_type = get_token().value;
        advance();

        proc->proc_name = get_token().value;
        advance();

        consume(TokenType::SYMBOL, "(");

        if(check(TokenType::SYMBOL, ")")) {
            advance();

            proc->body_node = parse_body();
            
            return proc;

        }else if(check(TokenType::VOID, "void")) {
            advance();
            consume(TokenType::SYMBOL, ")");
           
            proc->body_node = parse_body();

            return proc;
        }
    }

    throw std::runtime_error("Invalid proc return type - Use string, int, bool, double, char or void. it ain't that hard'");
}

std::unique_ptr<Node> Parser::parse_statement() {

    Token curr = get_token();
    
    switch (curr.type) {

        case TokenType::TEMPEST: {
            return parse_proc();
            break;
        }

        case TokenType::PROC: {
            return parse_proc();
            break;
        }

        case TokenType::STORM: {
            return parse_storm();
            break;
        }

        case TokenType::FOR: {
            return parse_for();
            break;
        }

        case TokenType::WHILE: {
            return parse_while();
            break;
        }

        case TokenType::RANGE: {
            return parse_range();
            break;
        }

        case TokenType::IF: {
            return parse_if();
            break;
        }

        case TokenType::BOOL: {
            return parse_variable();
            break;
        }

        case TokenType::CHAR: {
            return parse_variable();
            break;
        }

        case TokenType::SYMBOL: {
            break;
        }

        case TokenType::OPERATOR: {
            parse_condition();
            break;
        }

        case TokenType::UNARY_OP: {
            parse_condition();
            break;
        }

        case TokenType::UPTO: {
            parse_condition();
            break;
        }

        case TokenType::END_OF_FILE: {
            break;
        }

        default: {
            return parse_variable();
        }
    }
}

std::unique_ptr<StormNode> Parser::parse_storm() {

    auto storm = std::make_unique<StormNode>();

    consume(TokenType::STORM, "storm");
   
    Token name = advance();

    storm->storm_name = name.value;
    
    advance();
    
    consume(TokenType::SYMBOL, "{");
    
    do {
        storm->storm_statements.push_back(parse_variable());
    }while(!check(TokenType::SYMBOL, "}"));

    consume(TokenType::SYMBOL , "}");

    return storm;
}

std::unique_ptr<VariableNode> Parser::parse_variable() {

    auto var = std::make_unique<VariableNode>();

    var->name = get_token().value;
    advance();
    
    //prior declaration, now initializing e.g. x = 10;
    if(check(TokenType::OPERATOR, "=")) {
        advance();
        var->init = parse_condition();
        return var;
    }

    if(!check(TokenType::SYMBOL, ":")) {
        throw std::runtime_error("Variable name and type must be seperated by ':' Error on line: "
                                 + std::to_string(get_token().line) + " Column: " + std::to_string(get_token().col));
    }

    consume(TokenType::SYMBOL, ":");
    
    Token type = get_token();
    if(!is_variable_type(type.value)) {
        throw std::runtime_error("Please follow this syntax when declaring and/or initializing variables.\n Use: " 
                                 "x: int; or x: int = 10; or x = 10 if already declared");
    }

    var->type = type.value;
    advance();

    if(check(TokenType::SYMBOL, ";")) {
        advance();
        return var;
    }

    consume(TokenType::OPERATOR, "=");
    
    var->init = parse_condition();

    return var;
}

std::unique_ptr<BodyNode> Parser::parse_body() {

    auto body = std::make_unique<BodyNode>();

    consume(TokenType::SYMBOL, "{");

    do {
        
        body->statements.push_back(std::move(parse_statement()));

    }while(!check(TokenType::SYMBOL, "}"));

    consume(TokenType::SYMBOL, "}");

    return body;

}

std::unique_ptr<ForNode> Parser::parse_for() {

    auto for_node = std::make_unique<ForNode>();
    
    consume(TokenType::FOR, "for");
    consume(TokenType::SYMBOL, "(");
    
    //for(x: int = 0;i < 10;i++) { }

    if(check(TokenType::SYMBOL, ";")) {
        consume(TokenType::SYMBOL, ";");
    }else {
        for_node->init = parse_variable();
    }


    if(!check(TokenType::SYMBOL, ";")) {
        for_node->condition = parse_condition();
    }

    consume(TokenType::SYMBOL, ";");

    if(!check(TokenType::SYMBOL, ")")) {
        for_node->incr = parse_incr();
    }
    
    consume(TokenType::SYMBOL, ")");
     

    for_node->for_body = parse_body();

    return for_node;
    
}

std::unique_ptr<WhileNode> Parser::parse_while() {

    auto whle = std::make_unique<WhileNode>();

    consume(TokenType::WHILE, "while");
    consume(TokenType::SYMBOL, "(");

    whle->condition = parse_condition();
    advance();

    consume(TokenType::SYMBOL, ")");

    whle->while_body = parse_body();
    
    return whle;
}

std::unique_ptr<RangeNode> Parser::parse_range() {

    auto range = std::make_unique<RangeNode>();

    consume(TokenType::RANGE, "range");
    consume(TokenType::SYMBOL, "(");

    range->name = get_token().value;
    advance();
    
    consume(TokenType::OPERATOR, "=");
    
    range->condition = parse_condition();

    consume(TokenType::SYMBOL, ")");

    range->range_body = parse_body();

    return range;
}

std::unique_ptr<IfNode> Parser::parse_if() {

    auto i = std::make_unique<IfNode>();

    consume(TokenType::IF, "if");
    consume(TokenType::SYMBOL, "(");

    i->condition = parse_condition();

    consume(TokenType::SYMBOL, ")");

    i->if_body = parse_body();

    return i;
}

std::unique_ptr<Node> Parser::parse_incr() {

    Token name = get_token();
    if(name.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("For loop increment must start with a variable name");
    }

    advance();

    if(check(TokenType::UNARY_OP)) {
        Token op = get_token();

        if(op.value == "++" || op.value == "--") {
            advance();
            return std::make_unique<UnaryIncrNode>(name.value, op.value);
        }

    }
    
    if(check(TokenType::OPERATOR)) {
        Token op = get_token();

        if(op.value == "=" || op.value == "*=" || op.value == "*"
            || op.value == "/" || op.value == "/=" || op.value == "+=" || op.value == "+" || op.value == "-" || op.value == "-="){ 
            
            advance();

            auto value = parse_condition();

            auto node = std::make_unique<VariableNode>();
            node->name = name.value;//identifier
            node->op = op.value;//op
            node->init = std::move(value);//right side

            advance();
            return node;

        }
    }

    throw std::runtime_error("Invalid increment in for loop. Use i++, i-- or assignment");
    
}

std::unique_ptr<Condition> Parser::parse_condition() {
    return parse_add();
}

std::unique_ptr<Condition> Parser::parse_add() {

    std::unique_ptr<Condition> left = parse_mul();

    while(check(TokenType::OPERATOR, "+") || check(TokenType::OPERATOR, "-")) {
        Token op = get_token();
        advance();

        auto right = parse_mul();

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

    std::regex is_alpha("[a-zA-Z]");
    std::regex is_num("[0-9]");

    switch(curr.type) {

        case TokenType::STRING: {
            advance();            
            return std::make_unique<StringCondition>(curr);
            break;
        }

        case TokenType::DOUBLE: {
            advance(); 
            if(std::regex_search(curr.value, is_alpha)) {
                throw std::runtime_error("Characters are not allowed in integers"
                                         + std::to_string(curr.line) + " col: "
                                         + std::to_string(curr.col) + "\n");
            
            }

            return std::make_unique<DoubleCondition>(curr);
            break;
        }

        case TokenType::INTEGER: {
            advance(); 
            if(curr.value.find('.') || std::regex_search(curr.value, is_alpha)) {
                throw std::runtime_error("Decimal points or characters are not allowed in integers"
                                         + std::to_string(curr.line) + " col: "
                                         + std::to_string(curr.col) + "\n");
            }

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
            advance();
            return std::make_unique<IdentifierCondition>(curr);
        }
        //default to string is safest
        default:
            advance(); 
            return std::make_unique<StringCondition>(curr);

    };

}



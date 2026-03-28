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
        auto stmt = parse_statement();
        if(!stmt) break;
        main_node->globals.push_back(std::move(stmt));
    }
    
    return main_node;
}

std::unique_ptr<ProcedureNode> Parser::parse_proc() {

    auto proc = std::make_unique<ProcedureNode>();

    consume(TokenType::PROC, "proc");

    if(check(TokenType::STRING) || check(TokenType::INTEGER) || check(TokenType::DOUBLE) || check(TokenType::BOOL) || check(TokenType::CHAR) || check(TokenType::VOID)) {

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
        
        //proc parameters
        }else {

            bool exit {false};

            while(!check(TokenType::SYMBOL, ")")) {
                proc->parameters.push_back(parse_variable());
                
                if(check(TokenType::SYMBOL, ")")) {
                    consume(TokenType::SYMBOL, ")");
                    exit = true;
                }
                if(exit) break;
            }
        }

        proc->body_node = parse_body();

        return proc;
    }

    std::cerr << "End of parse proc" << get_token().value << "\n";
    consume(TokenType::SYMBOL, "}");

    throw std::runtime_error("Invalid proc return type - Use string, int, bool, double, char or void. it ain't that hard'");
}

std::unique_ptr<Node> Parser::parse_statement() {

    Token curr = get_token();

    std::cerr << "\nParse statement" << curr.value << "\n";
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

        case TokenType::RETURN: {
            return parse_return();
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

        case TokenType::IDENTIFIER: {
            Token next = peek_next(1);
            //these look ahead must be handled here and not in parse_variable
            //as its the only way to distinguish between unary operations, declarations and initializations
            if(next.value == "(") {
                auto call = parse_proc_call();
                consume(TokenType::SYMBOL, ";");
                return call;

            } else if(next.value == "++" || next.value == "--" || next.value == "=" ||
                      next.value == "+=" || next.value == "-=" || next.value == "*=" || 
                      next.value == "/=") {

                auto node = parse_incr();

                std::cerr << "STATEMENT IDENT " << get_token().value << "\n";
                consume(TokenType::SYMBOL, ";");
                
                return node;
            }

            return parse_variable();
        }

        case TokenType::SYMBOL: {
            break;
        }

        case TokenType::OPERATOR: {
            return parse_condition();
            break;
        }
            
        case TokenType::UNARY_OP: {
            return parse_condition();
            break;
        }

        case TokenType::UPTO: {
            return parse_condition();
            break;
        }

        case TokenType::END_OF_FILE: {
            advance();
            return nullptr;
        }

        default: {
            return parse_variable();
        }
    }
}

std::unique_ptr<StormNode> Parser::parse_storm() {

    std::cerr << "\nEntered storm\n";

    auto storm = std::make_unique<StormNode>();

    consume(TokenType::STORM, "storm");
   
    Token name = get_token();

    storm->storm_name = name.value;
    
    advance();
    
    consume(TokenType::SYMBOL, "{");
    
    while(!check(TokenType::SYMBOL, "}")){
        storm->storm_statements.push_back(parse_variable());

        if(check(TokenType::SYMBOL, ",") || check(TokenType::SYMBOL, ";")) advance();
    }
    consume(TokenType::SYMBOL , "}");
    consume(TokenType::SYMBOL, ";");

    return storm;
}

std::unique_ptr<VariableNode> Parser::parse_variable() {

    std::cerr << "Here: " << get_token().value << "\n";
    auto var = std::make_unique<VariableNode>();

    var->name = get_token().value;
    advance();

    //prior declaration, now initializing e.g. x = 10;
    if(check(TokenType::OPERATOR, "=")) {
        advance();
        var->init = parse_condition();
        consume(TokenType::SYMBOL, ";");
        return var;
    }

    std::cerr << get_token().value;
    if(!check(TokenType::SYMBOL, ":")) {
        throw std::runtime_error("Variable name and type must be seperated by ':' Error on line: "
                                 + std::to_string(get_token().line) + " Column: " + std::to_string(get_token().col));
    }

    consume(TokenType::SYMBOL, ":");
    
    Token type = get_token();
    var->type = type.value;
    advance();

    // storm struct initialization block
    if (check(TokenType::SYMBOL, "{")) {
        consume(TokenType::SYMBOL, "{");
        while(!check(TokenType::SYMBOL, "}")) {
            var->storm_init_fields.push_back(parse_condition());
            if(check(TokenType::SYMBOL, ",")) advance();
        }
        consume(TokenType::SYMBOL, "}");
        consume(TokenType::SYMBOL, ";");
        return var;
    }

    if(check(TokenType::SYMBOL, ";") || check(TokenType::SYMBOL, ",")) {
        advance();
        return var;
    }

    //end of storm
    if(check(TokenType::SYMBOL, "}")) {
        return var;
    }

    if(check(TokenType::SYMBOL, ")")) {
        return var;
    }

    std::cerr << "\nCurrent token: " << get_token().value << "\n";
    std::cerr << "here\n";
    consume(TokenType::OPERATOR, "=");
    std::cerr << "here\n";
    var->init = parse_condition();

    consume(TokenType::SYMBOL, ";");
    return var;
}

std::unique_ptr<BodyNode> Parser::parse_body() {

    auto body = std::make_unique<BodyNode>();

    consume(TokenType::SYMBOL, "{");

    while(!check(TokenType::SYMBOL, "}")) {
        body->statements.push_back(parse_statement());

        if(check(TokenType::SYMBOL, "}")) {
            break;
        }
    }

    std::cerr << "body";

    consume(TokenType::SYMBOL, "}");

    return body;

}

std::unique_ptr<ForNode> Parser::parse_for() {
    std::cerr << "Parse for: ";
    auto for_node = std::make_unique<ForNode>();

    std::cerr << get_token().value << "\n";

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

    std::cerr << "Parse while: " << get_token().value;
    
    consume(TokenType::SYMBOL, ")");

    whle->while_body = parse_body();
    
    return whle;
}

std::unique_ptr<RangeNode> Parser::parse_range() {

    std::cerr << "Entered range\n";
    auto range = std::make_unique<RangeNode>();

    consume(TokenType::RANGE, "range");
    consume(TokenType::SYMBOL, "(");

    range->name = get_token().value;
    advance();
    
    consume(TokenType::OPERATOR, "=");
    
    range->condition = parse_condition();
    
    consume(TokenType::UPTO, "..");
    range->end_val = parse_condition();

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

    std::cerr << "Parse incr: " << get_token().value << "Line" << get_token().line << "\n";
    if(name.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("For loop increment must start with a variable name");
    }

    advance();

    std::cerr << "Parse incr: " << get_token().value << "\n";
    
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

            std::cerr << "Token: " << get_token().value << "\n";
            advance();

            auto value = parse_condition();

            auto node = std::make_unique<VariableNode>();
            node->name = name.value;//identifier
            node->op = op.value;//op
            node->init = std::move(value);//right side

            return node;

        }
    }

    throw std::runtime_error("Invalid increment in for loop. Use i++, i-- or assignment");
    
}

std::unique_ptr<ReturnNode> Parser::parse_return() {

    auto rett = std::make_unique<ReturnNode>();

    consume(TokenType::RETURN, "return");

    rett->ret = parse_condition();

    consume(TokenType::SYMBOL, ";");
 
    return rett;
}

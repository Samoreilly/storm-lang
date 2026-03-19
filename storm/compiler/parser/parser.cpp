
#include "parser.h"
#include <memory>


std::unique_ptr<MainNode> Parser::construct_node() {

    auto main_node = std::make_unique<MainNode>();

    while(index < length) {
        main_node->globals.push_back(parse_statement());
    }
    
    return main_node;
}

std::unique_ptr<Node> parse_statement() {

}

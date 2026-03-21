#include "file_handler/load_files.h"
#include "compiler/lexical_analysis/lexer.h"
#include "compiler/parser/parser.h"
#include "transpiler/transpile.h"
#include "token.h"
#include <vector>
#include <string>

int main(void) {

    try{

        Load_Files l{};
        const std::string content = l.load_file();

        Lexer lex{content};

        lex.print_tokens();
        std::vector<Token>& tokens = lex.get_tokens();
    
        Parser parser{tokens};
        if (parser.root_node) {
            std::cout << "\n=== Abstract Syntax Tree ===\n";
            parser.root_node->print();
        }

        Transpile t{parser.get_ast()};
        t.transpile();


    
    }catch(const std::exception& e) {
        std::cerr << e.what();
    }
    return 0;
}

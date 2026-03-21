#include "file_handler/load_files.h"
#include "compiler/lexical_analysis/lexer.h"
#include "compiler/parser/parser.h"
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
    
    }catch(const std::exception& e) {
        std::cerr << e.what();
    }
    return 0;
}

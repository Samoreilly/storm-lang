#include "file_handler/load_files.h"
#include "compiler/lexical_analysis/lexer.h"
#include "token.h"
#include <vector>
#include <string>

int main(void) {

    Load_Files l{};
    const std::string content = l.load_file();

    Lexer lex{content};
    lex.print_tokens();

    std::vector<Token>& tokens = lex.get_tokens();
 

    return 0;
}

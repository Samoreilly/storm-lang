#include "file_handler/load_files.h"
#include "lexical_analysis/lexer.h"

int main(void) {

    Load_Files l{};
    const std::string content = l.load_file();

    Lexer lex{content};
    lex.print_tokens();

 
    return 0;
}

#include "file_handler/load_files.h"
#include "compiler/lexical_analysis/lexer.h"
#include "compiler/parser/parser.h"
#include "transpiler/transpile.h"
#include "compiler/semantic_analysis/semantic.h"
#include "token.h"
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

int main(void) {

    try{

        auto start_full = std::chrono::steady_clock::now();

        Load_Files l {};
        const std::string content = l.load_file();

        Lexer lex{content};

        lex.print_tokens();
        std::vector<Token>& tokens = lex.get_tokens();
    
        Parser parser{tokens};
        
        if (parser.root_node) {
            std::cout << "\n=== Abstract Syntax Tree ===\n";
            parser.root_node->print();
        }

        // Transpile t{parser.get_ast()};
        // const std::string code = t.transpile();

        // std::cerr << "writing to file";
        // l.write_to_file(code);
        // 
        // std::cout << "\n";
        // system("gcc transpiled.c && ./a.out");

        std::cerr << "\n\n=== SEMANTIC ANALYSIS ===\n\n";
        SymbolTable global("global-scope");

        //semantic_analysis
        int memory = 0;
    
        parser.root_node->analyze(&global, memory);

        std::string asm_code = parser.root_node->to_asm();

        auto end_full = std::chrono::steady_clock::now();

        auto full = end_full - start_full;

        std::cout << "Pipeline duration: " << full.count() << "\n";

        std::ofstream storm("storm.asm");

        storm << asm_code;

        storm.close();
    
    }catch(const std::exception& e) {
        std::cerr << e.what();
    }

    return 0;
}

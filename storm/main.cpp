#include "file_handler/load_files.h"
#include "compiler/lexical_analysis/lexer.h"
#include "compiler/parser/parser.h"
#include "transpiler/transpile.h"
#include "compiler/semantic_analysis/semantic.h"
#include "compiler/ir/ir.h"
#include "token.h"
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

//takes in files, folder
int main(int argc, char* argv[]) {

    try {

        auto start_full = std::chrono::steady_clock::now();

        std::vector<std::string> file_contents;
        
        Load_Files l {};
        
        if(argc <= 1) {
            file_contents = l.load_folder();
        }else {
            file_contents = l.load_files(argv, argc);
        }

        Lexer lex;
        for(const auto& content : file_contents) {
            std::vector<Token> tok = lex.lex(content); 
            
            tok.push_back({TokenType::END_OF_FILE, "END_OF_FILE"});
        }

        std::vector<std::vector<Token>>& file_tokens = lex.get_all_tokens();
        
        lex.print_tokens();
       
        auto master_node = std::make_unique<MainNode>();

        for(auto& file_token : file_tokens) {        
            Parser p{file_token};
       
            //merge all nodes into master_node
            for(auto& sub_global : p.root_node->globals) {
                master_node->globals.push_back(std::move(sub_global));
            }
         
        }

        std::cout << "\n";
        master_node->print();

        std::cerr << "\n\n=== SEMANTIC ANALYSIS ===\n\n";

        //semantic_analysis
        int memory = 0;
    
        SymbolTable master_table("master-node");
        
        master_node->analyze(&master_table, memory);
   
        //intermediate representation
        Ir ir{master_node};
        ir.gen_ir(ir);
        ir.print();
       

        std::string asm_code = master_node->to_asm();

        std::ofstream storm("storm.asm");

        storm << asm_code;

        storm.close();

        std::cout << "Pipeline done";

        return 0;

    //     std::string asm_code = parser.root_node->to_asm();

    //     auto end_full = std::chrono::steady_clock::now();

    //     auto full = end_full - start_full;

    //     std::cout << "Pipeline duration: " << full.count() << "\n";

    //     std::ofstream storm("storm.asm");

    //     storm << asm_code;

    //     storm.close();
    // 
    }catch(const std::exception& e) {
        std::cerr << e.what();
    }

    return 0;
}

#include "transpile.h"
#include "../compiler/parser/node.h"
#include <iostream>
#include <string>



void Transpile::transpile() {

    inclds.clear();
    
   std::string code = root->to_c();

   std::string header_block;
    for (const auto& header : inclds) {
        header_block += header + "\n";
    }
    
    if (!header_block.empty()) {
        code = header_block + "\n" + code;
    }

    //moves void main to bottom of code
    size_t start = code.find("void main");
    if (start != std::string::npos) {
        size_t brace_count = 0;
        bool started = false;
        size_t end = std::string::npos;

        for (size_t i = start; i < code.length(); i++) {
            if (code[i] == '{') {
                brace_count++;
                started = true;
            } else if (code[i] == '}') {
                brace_count--;
                if (started && brace_count == 0) {
                    end = i;
                    break;
                }
            }
        }

        if (end != std::string::npos) {
            std::string main_func = code.substr(start, end - start + 1);
            code.erase(start, end - start + 1);
            code += "\n\n" + main_func;
        }
    }

    std::cout << code << std::endl;
}

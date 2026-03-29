#pragma once

#include "../ir/ir.h"
#include "regalloc.h"
#include <iostream>
#include <vector>
#include <map>


static std::string data = "section .data \n\t";

// double and are string are too large so we store them in section .data
// with unique names
static int string_counter;
static int double_counter;
static int if_counter;//to have unique subroutine names for jumps
static std::string current_exit_label = "";

class Backend {

    std::vector<Instruction> instructions;
    SymbolTable* table;
    RegAlloc& ra;

public:

    Backend();
    Backend(std::vector<Instruction>& i, SymbolTable* t, RegAlloc& r)
                                    : instructions(i), table(t), ra(r)  {}

    std::string gen_asm();

    inline std::string get_addr(Address addr) {

        if (addr.type == ADDR_TYPE::CONSTANT) {
            if (addr.data_type == "string") {
        
                static std::map<std::string, std::string> str_map;
                if (!str_map.count(addr.name)) {
                    std::string label = "string_" + std::to_string(string_counter++);
                    std::string raw_str = addr.name;
                    // ensure string is wrapped in backticks for nasm escape support
                    if (!raw_str.empty()) {
                        if (raw_str.front() == '"' && raw_str.back() == '"') {
                            raw_str.front() = '`';
                            raw_str.back() = '`';
                        } else if (raw_str.front() != '`') {
                            raw_str = "`" + raw_str + "`";
                        }
                    }
                    data += label + ": db " + raw_str + ", 0\n\t";
                    str_map[addr.name] = label;
                }
                
                return str_map[addr.name];
            
            } else if (addr.data_type == "double") {
                static std::map<std::string, std::string> dbl_map;
                if (!dbl_map.count(addr.name)) {
                    std::string label = "double_" + std::to_string(double_counter++);
                    data += label + ": dq " + addr.name + "\n\t";
                    dbl_map[addr.name] = label;
                }
                return "qword [" + dbl_map[addr.name] + "]";
            }
            return addr.name;
        }

        // check register allocator first
        std::string assigned = ra.get_reg_for(addr.name);
        if (assigned != "stack" && assigned != "-1") {
            return assigned;
        }

        // Use pre-calculated offset for all variables/temps/params
        std::string sign = (addr.offset >= 0) ? "+" : "";
        return "qword [rbp " + sign + std::to_string(addr.offset) + "]";
    }

};

#pragma once

#include "../parser/node.h"
#include "address.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

//quadruple address code
struct Instruction {
    Address result;
    Address left_operand;
    OPCODE op;
    Address right_operand;
};

struct Metadata {
    Instruction instr;
    int vec_index;
    bool is_used;

    Metadata() {}
    Metadata(Instruction i, bool u, int idx) : instr(i), is_used(u), vec_index(idx) {}
};

class Ir {

public:

    //used to remove unused variables
    std::unordered_map<std::string, Metadata> variables;
    std::vector<size_t> _delete;

    void remove_unused_variables() {
        std::cerr << "Entered\n\n"; 
        for (auto it = variables.begin(); it != variables.end(); ) {
            if (!it->second.is_used) {
                std::cerr << "\n\nREMOVED VARIABLE" << it->second.instr.result.name << "\n";
                _delete.push_back(it->second.vec_index);
                it = variables.erase(it);
            } else {
                ++it;
            }
        }

        std::sort(_delete.rbegin(), _delete.rend());

        for (size_t idx : _delete) {
            instructions.erase(instructions.begin() + idx);
        }
    }

    std::vector<Instruction> instructions;
    std::unique_ptr<MainNode>& master_node;
    
    //set to true when a return node is found, reset when a label is found
    bool is_unreachable {false};

    int temp_counter = 0;// t1, t2, t3, will be set to 1 once scope is changed
    int label_counter = 0;
    int proc_counter = 0;
    std::string curr_proc = "main";// for tail call optimization

    Ir(std::unique_ptr<MainNode>& main) : master_node(main) {}

    Address gen_ir(Ir& context);

    std::vector<Address> curr_params;
    void tco(Ir& context);
    
    void print();
    
    inline Address get_temp() {
        return {ADDR_TYPE::TEMP, "t" + std::to_string(temp_counter++), " "};
    }

    inline Address get_label() {
        return {ADDR_TYPE::CONSTANT, "L" + std::to_string(label_counter++), ""};
    }

    inline Address get_proc_label() {
        return {ADDR_TYPE::CONSTANT, "P" + std::to_string(proc_counter++), ""};
    }

    void emit(Address res, Address L, OPCODE op, Address R) {

        if(variables.count(L.name)) variables[L.name].is_used = true;
        if(variables.count(R.name)) variables[R.name].is_used = true;

        if(res.type == ADDR_TYPE::VARIABLE && op != OPCODE::LABEL
            && op != OPCODE::ARG && op != OPCODE::PARAM && op != OPCODE::CALL) {
            if(variables.count(res.value)) {
                if(!variables[res.value].is_used) {
                    //if curr var was declared/initialized but not used, remove it
                    _delete.push_back(variables[res.value].vec_index);
                }
            }

            int current_index = instructions.size();
            variables[res.value] = Metadata{{res, L, op, R}, false, current_index}; 
        }
            
        //set everthing prior to used, so its not deleted
        if (op == OPCODE::LABEL || op == OPCODE::GOTO || op == OPCODE::IF_FALSE) {
            for (auto& pair : variables) {
                pair.second.is_used = true;
            }
        }

        if(op == OPCODE::LABEL) {
            is_unreachable = false;
        }

        if(!is_unreachable) {
            instructions.push_back({res, L, op, R});
        }

        if(op == OPCODE::RETURN || op == OPCODE::GOTO) {
            is_unreachable = true;
        }
   

    }

    void emitLabel(Address label) {
        emit(label, Address{}, OPCODE::LABEL, Address{});
    }

    void emitParam() {
        Address param{ADDR_TYPE::PARAM, "param", "param"};
        emit(param, param, OPCODE::PARAM, param);
    }

    //if false it jumps to end of loop
    void emit_if_false(Address cond, Address label) {
        emit(label, cond, OPCODE::IF_FALSE, Address{});
    }

    void emitGoto(Address target) {
        emit(Address{}, Address{}, OPCODE::GOTO, target);
    }
};

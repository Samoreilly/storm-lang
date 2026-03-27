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

class Ir {

public:

    std::vector<Instruction> instructions;
    std::unique_ptr<MainNode>& master_node;

    //set to true when a return node is found, reset when a label is found
    bool is_unreachable {false};

    int temp_counter = 0;// t1, t2, t3, will be set to 1 once scope is changed
    int label_counter = 0;
    int proc_counter = 0;

    Ir(std::unique_ptr<MainNode>& main) : master_node(main) {}

    Address gen_ir(Ir& context);
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

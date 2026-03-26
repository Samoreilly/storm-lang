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
        instructions.push_back({res, L, op, R});
    }

    void emitLabel(Address label) {
        instructions.push_back({label, Address{}, OPCODE::LABEL, Address{}});
    }

    void emitParam() {
        Address param{ADDR_TYPE::PARAM, "param", "param"};
        instructions.push_back({param, param, OPCODE::PARAM, param});
    }

    //if false it jumps to end of loop
    void emit_if_false(Address cond, Address label) {
        instructions.push_back({label, cond, OPCODE::IF_FALSE, Address{}});
    }

    void emitGoto(Address target) {
        instructions.push_back({Address{}, Address{}, OPCODE::GOTO, target});
    }
};

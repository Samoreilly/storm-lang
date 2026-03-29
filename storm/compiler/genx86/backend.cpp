#include "backend.h"

std::string Backend::gen_asm() {

    std::string main_code = "extern printf\nglobal main\n\nsection .text\n";
    std::string code;

    std::vector<std::string> int_regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    std::vector<std::string> double_regs = {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"};

    int int_arg_idx = 0;
    int double_arg_idx = 0;

    int int_param_idx = 0;
    int double_param_idx = 0;

    for(const auto& instr : instructions) {
        
        switch (instr.op) {
       
            case OPCODE::ADD: {
                if(instr.result.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    if (instr.right_operand.data_type != "double") {
                        code += "\tcvtsi2sd xmm1, " + get_addr(instr.right_operand) + "\n";
                        code += "\taddsd xmm0, xmm1\n";
                    } else {
                        code += "\taddsd xmm0, " + get_addr(instr.right_operand) + "\n";
                    }
                    code += "\tmovsd " + get_addr(instr.result) + ", xmm0\n";
               }else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tadd rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tmov " + get_addr(instr.result) + ", rax\n";
                }
                break;
            }
            
            case OPCODE::MINUS: {
                if(instr.result.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    if (instr.right_operand.data_type != "double") {
                        code += "\tcvtsi2sd xmm1, " + get_addr(instr.right_operand) + "\n";
                        code += "\tsubsd xmm0, xmm1\n";
                    } else {
                        code += "\tsubsd xmm0, " + get_addr(instr.right_operand) + "\n";
                    }
                    code += "\tmovsd " + get_addr(instr.result) + ", xmm0\n";
               }else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tsub rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tmov " + get_addr(instr.result) + ", rax\n";
                }
                break;
            }
            
            case OPCODE::MUL: {
                if(instr.result.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    if (instr.right_operand.data_type != "double") {
                        code += "\tcvtsi2sd xmm1, " + get_addr(instr.right_operand) + "\n";
                        code += "\tmulsd xmm0, xmm1\n";
                    } else {
                        code += "\tmulsd xmm0, " + get_addr(instr.right_operand) + "\n";
                    }
                    code += "\tmovsd " + get_addr(instr.result) + ", xmm0\n";
               }else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\timul rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tmov " + get_addr(instr.result) + ", rax\n";
                }
                break;
            }
            
            case OPCODE::DIV: {
                if(instr.result.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    if (instr.right_operand.data_type != "double") {
                        code += "\tcvtsi2sd xmm1, " + get_addr(instr.right_operand) + "\n";
                        code += "\tdivsd xmm0, xmm1\n";
                    } else {
                        code += "\tdivsd xmm0, " + get_addr(instr.right_operand) + "\n";
                    }
                    code += "\tmovsd " + get_addr(instr.result) + ", xmm0\n";
               }else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcqo\n";
                    code += "\tidiv qword " + get_addr(instr.right_operand) + "\n";
                    code += "\tmov " + get_addr(instr.result) + ", rax\n";
                }
                break;
            }
            
            case OPCODE::LT: {
                if(instr.left_operand.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcomisd xmm0, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetb al\n"; // below = less than for double
                } else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcmp rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetl al\n"; // less than for signed int
                }
                code += "\tmovzx rax, al\n";
                code += "\tmov " + get_addr(instr.result) + ", rax\n";
                break;
            }
            
            case OPCODE::GT: {
                if(instr.left_operand.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcomisd xmm0, " + get_addr(instr.right_operand) + "\n";
                    code += "\tseta al\n"; // above = greater than for double
                } else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcmp rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetg al\n"; // greater than for signed int
                }
                code += "\tmovzx rax, al\n";
                code += "\tmov " + get_addr(instr.result) + ", rax\n";
                break;
            }
            
            case OPCODE::LOE: {
                if(instr.left_operand.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcomisd xmm0, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetbe al\n"; // below or equal
                } else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcmp rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetle al\n"; // less or equal for signed int
                }
                code += "\tmovzx rax, al\n";
                code += "\tmov " + get_addr(instr.result) + ", rax\n";
                break;
            }
            
            case OPCODE::GOE: {
                if(instr.left_operand.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcomisd xmm0, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetae al\n"; // above or equal
                } else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcmp rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetge al\n"; // greater or equal for signed int
                }
                code += "\tmovzx rax, al\n";
                code += "\tmov " + get_addr(instr.result) + ", rax\n";
                break;
            }

            case OPCODE::EQ: {
                if(instr.left_operand.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcomisd xmm0, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsete al\n"; 
                } else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcmp rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsete al\n"; 
                }
                code += "\tmovzx rax, al\n";
                code += "\tmov " + get_addr(instr.result) + ", rax\n";
                break;
            }

            case OPCODE::NEQ: {
                if(instr.left_operand.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcomisd xmm0, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetne al\n"; 
                } else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tcmp rax, " + get_addr(instr.right_operand) + "\n";
                    code += "\tsetne al\n"; 
                }
                code += "\tmovzx rax, al\n";
                code += "\tmov " + get_addr(instr.result) + ", rax\n";
                break;
            }
            
            case OPCODE::RETURN: {
                if (instr.right_operand.name != "") {
                    // return value in rax or xmm0 based on standard abi
                    if (instr.right_operand.data_type == "double") {
                        code += "\tmovsd xmm0, " + get_addr(instr.right_operand) + "\n";
                    } else {
                        code += "\tmov rax, " + get_addr(instr.right_operand) + "\n";
                    }
                }
                // emit standard stack frame teardown
                code += "\tlea rsp, [rbp - 48]\n"; // jump over local variables to saved registers
                code += "\tpop r11\n"; // stack alignment padding
                code += "\tpop r12\n";
                code += "\tpop r13\n";
                code += "\tpop r14\n";
                code += "\tpop r15\n";
                code += "\tpop rbx\n";
                code += "\tleave\n";
                code += "\tret\n";
                break;
            }
            
            case OPCODE::GOTO: {
                code += "\tjmp " + instr.right_operand.name + "\n";
                break;
            }
            
            case OPCODE::IF_FALSE: {
                code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                code += "\tcmp rax, 0\n";
                code += "\tje " + instr.result.name + "\n"; // jump if truthy value is false (0)
                break;
            }
            
            case OPCODE::ASSIGN: {
                if(instr.result.data_type == "double") {
                    code += "\tmovsd xmm0, " + get_addr(instr.left_operand) + "\n";
                    code += "\tmovsd " + get_addr(instr.result) + ", xmm0\n";
                } else {
                    code += "\tmov rax, " + get_addr(instr.left_operand) + "\n";
                    code += "\tmov " + get_addr(instr.result) + ", rax\n";
                }
                break;
            }
            
            case OPCODE::LABEL: {
                code += instr.result.name + ":\n";
                // reset param counters when a new function/label starts
                int_param_idx = 0;
                double_param_idx = 0;

                // add function prologue if this label is a registered function
                SymbolEntry* fn_entry = table->lookup(instr.result.name);
                if (fn_entry && fn_entry->is_function) {
                     //push fp and set new fp
                     code += "\tpush rbp\n";
                     code += "\tmov rbp, rsp\n";
                     code += "\tpush rbx\n";
                     code += "\tpush r15\n";
                     code += "\tpush r14\n";
                     code += "\tpush r13\n";
                     code += "\tpush r12\n";
                     code += "\tpush r11\n"; // stack alignment padding
                     
                     // aligns stack for local variables
                     int alloc = -(fn_entry->stack_frame_size);
                     if (alloc % 16 != 0) alloc += (16 - (alloc % 16));
                     if (alloc > 0) code += "\tsub rsp, " + std::to_string(alloc) + "\n";
                }
                break;
            }
            
            case OPCODE::CALL: {
               
                code += "\tmov al, " + std::to_string(double_arg_idx) + "\n";
                code += "\tcall " + instr.left_operand.name + "\n";

                // return values are stored in rax
                if (instr.result.name != "" && instr.result.data_type != "void") {
                    if (instr.result.data_type == "double") {
                        code += "\tmovsd " + get_addr(instr.result) + ", xmm0\n";
                    } else {
                        code += "\tmov " + get_addr(instr.result) + ", rax\n";
                    }
                }

                // reset arg counters after a call is complete
                int_arg_idx = 0;
                double_arg_idx = 0;
                break;
            }
            
            case OPCODE::PARAM: {
                if (instr.result.data_type == "double") {
                    if (double_param_idx < double_regs.size()) {
                        code += "\tmovsd " + get_addr(instr.result) + ", " + double_regs[double_param_idx++] + "\n";
                    } else {
                        
                    }
                } else {
                    if (int_param_idx < int_regs.size()) {
                        code += "\tmov " + get_addr(instr.result) + ", " + int_regs[int_param_idx++] + "\n";
                    }
                }
                break;
            }
            
            case OPCODE::ARG: {
                if (instr.left_operand.data_type == "double") {
                    if (double_arg_idx < double_regs.size()) {
                        code += "\tmovsd " + double_regs[double_arg_idx++] + ", " + get_addr(instr.left_operand) + "\n";
                    }
                } else {

                    if (int_arg_idx < int_regs.size()) {
                        code += "\tmov " + int_regs[int_arg_idx++] + ", " + get_addr(instr.left_operand) + "\n";
                    }
                }
                break;
            }
            
            case OPCODE::STORM_DEF: {
                break;
            }
            
            case OPCODE::STORM_INIT: {
                break;
            }
        }
    }

    return data + "\n\n" + main_code + code;
}

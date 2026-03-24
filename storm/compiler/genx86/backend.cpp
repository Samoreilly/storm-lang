#include "../semantic_analysis/semantic.h"
#include "backend.h"
#include "../parser/node.h"
#include "../parser/condition.h"
#include <iostream>
#include <string>


std::string MainNode::to_asm() {
    std::string final_code;
   
    //headers
    final_code += "extern printf\n";
    final_code += "section .text\n";
    final_code += "global main\n\n";

    //functions/globals
    for(const auto& node : globals) {
        if(node) final_code += node->to_asm();
    }

    
    final_code += "\nsection .data\n";
    final_code += "format_int: db \"%d\", 10, 0\n";// print statement
    final_code += "format_str: db \"%s\", 10, 0\n";// print string statement
    final_code += "format_dbl: db \"%f\", 10, 0\n";// print double statement
    final_code += data;

    return final_code;
}
std::string BodyNode::to_asm() {
    std::string code;

    for(const auto& node : statements) {
        if(node) code += node->to_asm();
    }

    return code;
}

std::string VariableNode::to_asm() {
    std::string code;

    if(init) code += init->to_asm();

    code += "pop rax\n";

    //store value in new slot
    code += "mov [rbp + " + std::to_string(this->saved_offset) + "], rax\n";

    return code;
}

std::string ProcedureNode::to_asm() {
    std::string code;
    // create a unique exit door for this function
    std::string my_exit = proc_name + "_EXIT_" + std::to_string(if_counter++);
    std::string prev_exit = current_exit_label;
    current_exit_label = my_exit;

    code += proc_name + ":\n";
    code += "push rbp\n";
    code += "mov rbp, rsp\n";

    // stack alignment: linux requires rsp to be a multiple of 16 before a call
    int size = std::abs(stack_frame_size);
    int aligned_size = (size % 16 == 0) ? size : size + (16 - size % 16);
    code += "sub rsp, " + std::to_string(aligned_size) + "\n";

    // save register arguments into their basement slots (negative offsets)
    std::vector<std::string> arg_regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    for (int i = 0; i < (int)parameters.size() && i < 6; ++i) {
        int offset = -((i + 1) * 8);
        code += "mov [rbp + " + std::to_string(offset) + "], " + arg_regs[i] + "\n";
    }

    code += body_node->to_asm();

    // all returns jump here
    code += my_exit + ":\n";
    code += "mov rsp, rbp\n";
    code += "pop rbp\n";
    code += "ret\n";
    
    current_exit_label = prev_exit;
    return code;
}

std::string ProcCallNode::to_asm() {
    std::string code;
    
    //parameter registersI
    std::vector<std::string> arg_regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

    for (const auto& arg : arguments) {
        if (arg) code += arg->to_asm();
    }

    for (int i = arguments.size() - 1; i >= 0; --i) {
        if (i < 6) code += "pop " + arg_regs[i] + "\n";
    }

    if (proc_name == "echo") {
        code += "extern printf\n";
        
        // selects format string based on argument type
        if (!arguments.empty() && arguments[0]->getType() == "string") {
            code += "mov rsi, rdi\n"; 
            code += "lea rdi, [format_str]\n";
            code += "mov rax, 0\n"; 
        } else if (!arguments.empty() && arguments[0]->getType() == "double") {
            code += "movq xmm0, rdi\n";
            code += "lea rdi, [format_dbl]\n"; 
            code += "mov rax, 1\n"; 
        } else {
            code += "mov rsi, rdi\n"; 
            code += "lea rdi, [format_int]\n"; 
            code += "mov rax, 0\n"; 
        }

        code += "call printf\n";
    } else {
        code += "mov rax, 0\n"; // 0 xmm registers for normal proc calls unless they take floats
        code += "call " + proc_name + "\n";
    }

    if (actual_type != "void") {
        code += "push rax\n";
    }
    return code;

}

std::string StormNode::to_asm() {
    return "";
}

std::string IfNode::to_asm() {
    std::string code;

    code += condition->to_asm();

    code += "pop rax\n";//get the true or false from the binary expression
    code += "cmp rax, 0\n";//sets flag to 1 if rax is 0;
    
    std::string if_label("IF_" + std::to_string(if_counter++));

    code += "je "+ if_label + "\n";//jump past body if condition is false

    code += if_body->to_asm();
    
    code += if_label + ":\n";

    return code;
}

std::string WhileNode::to_asm() {
    std::string code;
    
    std::string start_while("IF_" + std::to_string(if_counter++));
    std::string end_while("IF_" + std::to_string(if_counter++));

    code += start_while + ":\n";

    code += condition->to_asm();

    code += "pop rax\n";//get the true or false from the binary expression
    code += "cmp rax, 0\n";//if rax is 0, it sets the zero flag
    
    code += "je " + end_while + "\n";//jump past body if zero flag is set
    code += while_body->to_asm();

    code += "jmp " + start_while + "\n";
    code += end_while + ":\n";


    return code;
}

std::string ForNode::to_asm() {

    //if statement counter is just for everything cuz why not
    std::string code;
    std::string start_label = "FOR_START_" + std::to_string(if_counter++);
    std::string end_label = "FOR_END_" + std::to_string(if_counter++);

    // initialize loop variable
    if (init && init.value()) code += init.value()->to_asm();

    code += start_label + ":\n";

    // check condition if it exists
    if (condition && condition.value()) {
        code += condition.value()->to_asm();
        code += "pop rax\n";
        code += "cmp rax, 0\n";
        code += "je " + end_label + "\n"; // exit if false
    }

    // run loop body
    code += for_body->to_asm();

    // run increment step
    if (incr && incr.value()) code += incr.value()->to_asm();

    code += "jmp " + start_label + "\n";
    code += end_label + ":\n";

    return code;
}

std::string RangeNode::to_asm() {
    std::string code;
    std::string start_label = "RANGE_START_" + std::to_string(if_counter++);
    std::string end_label = "RANGE_END_" + std::to_string(if_counter++);

    // set initial value of iterator
    code += condition->to_asm();
    code += "pop rax\n";
    code += "mov [rbp + " + std::to_string(saved_offset) + "], rax\n";

    code += start_label + ":\n";

    // check if iterator < end_val
    code += end_val->to_asm();
    code += "mov rax, [rbp + " + std::to_string(saved_offset) + "]\n";
    code += "pop rbx\n";
    code += "cmp rax, rbx\n";
    code += "jge " + end_label + "\n";

    code += range_body->to_asm();

    // increment iterator
    code += "mov rax, [rbp + " + std::to_string(saved_offset) + "]\n";
    code += "inc rax\n";
    code += "mov [rbp + " + std::to_string(saved_offset) + "], rax\n";

    code += "jmp " + start_label + "\n";
    code += end_label + ":\n";
    return code;
}

std::string UnaryIncrNode::to_asm() {
    std::string code;

    // load variable into rax using saved offset
    code += "mov rax, [rbp + " + std::to_string(saved_offset) + "]\n";

    if(op == "++") {
        code += "inc rax\n"; // increment
    }else if(op == "--") {
        code += "dec rax\n"; // decrement
    }

    // save updated value back to stack
    code += "mov [rbp + " + std::to_string(saved_offset) + "], rax\n";

    return code;
}

std::string BinaryExpression::to_asm() {
    std::string code;
   

    code += left->to_asm() + "\n";
    code += right->to_asm() + "\n";
    
    code += "pop rbx\n";
    code += "pop rax\n";

    if(op == "<") {
        code += "cmp rax, rbx\n";
        code += "setl al\n"; //sets the al register to 0/1 based on the last comparison
        code += "movzx rax, al\n"; //moves bits to rax and filling as al is only 8 bits
        code += "push rax\n";

        return code;

    }else if(op == ">") {
        code += "cmp rax, rbx\n";
        code += "setg al\n";
        code += "movzx rax, al\n";
        code += "push rax\n";

        return code;
    
    }else if(op == "==") {
        code += "cmp rax, rbx\n";
        code += "sete al\n";
        code += "movzx rax, al\n";
        code += "push rax\n";
        
        return code;
    
    }else if(op == "<=") {
        code += "cmp rax, rbx\n";
        code += "setle al\n";
        code += "movzx rax, al\n";
        code += "push rax\n";
        
        return code;
        
    }else if(op == ">=") {
        code += "cmp rax, rbx\n";
        code += "setge al\n";
        code += "movzx rax, al\n";
        code += "push rax\n";
        
        return code;
    }

    if(op == "/") {
        code += "cqo\n"; //extends the register into rdx spanning 128 bits
        code += "idiv rbx\n"; // rax is a special reg, so idiv knows about it rax = rax / rbx
        code += "push rax\n"; 
        return code;
    }

    std::string operand;
    if(op == "*") {
        operand = "imul";
    }else if(op == "+") {
        operand = "add";
    }else if(op == "-") {
        operand = "sub";
    }

    code += operand + " rax, " + "rbx\n";
    code += "push rax\n";

    return code;
    
}

std::string IntegerCondition::to_asm() {
    std::string code;

    code += "push " + token.value + "\n";

    return code;
}

std::string DoubleCondition::to_asm() {

    std::string label = "DBL_" + std::to_string(double_counter++);
  
    std::string data_line = label + ":  dq " + token.value + "\n";
    data += data_line;

    //loads value into rax from the data section label
    std::string code = "mov rax, [" + label + "]\n";
    //push onto stack
    code += "push rax\n";

    return code;
}

std::string BoolCondition::to_asm() {
    std::string code;

    std::string bin = token.value == "true" ? "1" : "0";
    code += "push " + bin + "\n";
    
    return code;
}

std::string StringCondition::to_asm() {
  
    std::string label = "STR_" + std::to_string(string_counter++);
  
    std::string data_line = label + ": db \"" + token.value + "\", 10, 0\n";
    data += data_line;

    //loads address into rax
    std::string code = "lea rax, [" + label + "]\n";
    //push onto stack
    code += "push rax\n";

    return code;
}

std::string CharCondition::to_asm() {
    return "";
}

std::string IdentifierCondition::to_asm() {
    std::string code;
    code += "mov rax, [rbp + " + std::to_string(this->saved_offset) + "]\n";
    code += "push rax\n";
    return code;
}

std::string ReturnNode::to_asm() {
    std::string code;
    if (ret) {
        code += ret->to_asm(); // Pushes result to stack
        code += "pop rax\n";   // Function returns always go in RAX
    }
    
    // Jump to the exit label of the current function to clean up
    if (!current_exit_label.empty()) {
        code += "jmp " + current_exit_label + "\n";
    }
    
    return code;
}




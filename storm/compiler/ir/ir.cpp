#include "ir.h"
#include "../parser/node.h"
#include "address.h"

Address Ir::gen_ir(Ir& context) {
    if(master_node) {
        return master_node->gen_ir(context);
    }
    return {};
}

static std::string opToString(OPCODE op) {
    switch (op) {
        case OPCODE::ADD: return "ADD";
        case OPCODE::MINUS: return "SUB";
        case OPCODE::MUL: return "MUL";
        case OPCODE::DIV: return "DIV";
        case OPCODE::LT: return "LT";
        case OPCODE::GT: return "GT";
        case OPCODE::LOE: return "LE";
        case OPCODE::GOE: return "GE";
        case OPCODE::RETURN: return "RET";
        case OPCODE::GOTO: return "GOTO";
        case OPCODE::IF_FALSE: return "IF_FALSE";
        case OPCODE::ASSIGN: return "ASSIGN";
        case OPCODE::LABEL: return "LABEL";
        case OPCODE::CALL: return "CALL";
        case OPCODE::PARAM: return "PARAM";
        case OPCODE::ARG: return "ARG";
        default: return "UNKNOWN";
    }
}

void Ir::print() {
    std::cout << "\n--- Generated IR ---\n";
    for (const auto& inst : instructions) {
        if (inst.op == OPCODE::LABEL) {
            std::cout << inst.result.name << ":" << "\n";
            continue;
        }

        std::cout << "  ";
        if (inst.op == OPCODE::GOTO) {
            std::cout << "GOTO " << inst.right_operand.name;
        } else if (inst.op == OPCODE::IF_FALSE) {
            std::cout << "IF_FALSE " << inst.left_operand.name << " GOTO " << inst.result.name;
        } else if (inst.op == OPCODE::ASSIGN) {
            std::cout << inst.result.name << " = " << inst.left_operand.name;
        } else if (inst.op == OPCODE::CALL) {
             if (!inst.result.name.empty()) std::cout << inst.result.name << " = ";
             std::cout << "CALL " << inst.left_operand.name;
        } else if (inst.op == OPCODE::ARG || inst.op == OPCODE::PARAM) {
            std::cout << opToString(inst.op) << " " << inst.result.name;
        } else if (inst.op == OPCODE::RETURN) {
            std::cout << "RETURN " << inst.right_operand.name;
        } else {
            // math logic: res = L op R
            std::cout << inst.result.name << " = " << inst.left_operand.name 
                      << " " << opToString(inst.op) << " " << inst.right_operand.name;
        }
        std::cout << std::endl;
    }
    std::cout << "--------------------\n" << "\n";
}

Address MainNode::gen_ir(Ir& context) {
    for(auto& node : globals) {
        if(node) node->gen_ir(context);
    }
    return {};
}

Address BodyNode::gen_ir(Ir& context) {
    
    for(auto& node : statements) {
        if(node) node->gen_ir(context);
    }

    return {};
}

Address IntegerToDoubleNode::gen_ir(Ir& context) {
    return node->gen_ir(context);
}

Address DoubleToIntegerNode::gen_ir(Ir& context) {
    return node->gen_ir(context);
}

Address VariableNode::gen_ir(Ir& context) {

    if(init) {

        Address in = init->gen_ir(context);
        Address var(ADDR_TYPE::VARIABLE, name , name);
        
        std::cerr << name << name << name;
        //turns x *= 5 into x = x * 5
        if(op.has_value()) {
            std::string& op = this->op.value();

            if(op == "*=") {
                context.emit(var, var, OPCODE::MUL, in);
            }else if(op == "+=") {
                context.emit(var, var, OPCODE::ADD, in);
            }else if(op == "/=") {
                context.emit(var, var, OPCODE::DIV, in);
            }else if(op == "-=") {
                context.emit(var, var, OPCODE::MINUS, in);
            }

        }else {
            //x = 5;
            if(!context.instructions.empty() && dynamic_cast<ProcCallNode*>(init.get())) {
                Instruction& last = context.instructions.back();
                last.result = var;
            }else {
                context.emit(var, in, OPCODE::ASSIGN, Address{});
            }
        }
    }

    return {};
}

Address ProcedureNode::gen_ir(Ir& context) {

    Address start_proc(ADDR_TYPE::VARIABLE, proc_name, proc_name);
    context.emitLabel(start_proc);

    for(const auto& param : parameters) {                
        Address p_addr(ADDR_TYPE::PARAM, param->name, param->name);
        context.emit(p_addr, p_addr, OPCODE::PARAM, p_addr);
    }

    body_node->gen_ir(context);

    return {};
}

Address ProcCallNode::gen_ir(Ir& context) {

    for(const auto& args : arguments) {
        Address arg = args->gen_ir(context);        
        context.emit(arg, arg, OPCODE::ARG, arg);
    }

    Address res_proc = Address{};

    if(actual_type != "void") {
        res_proc = context.get_temp();
    }

    Address proc_label(ADDR_TYPE::VARIABLE, proc_name, proc_name);

    //e.g. t1 = get_sum(1, 2);
    context.emit(res_proc, proc_label, OPCODE::CALL, Address{});

    return res_proc;
}

Address StormNode::gen_ir(Ir& context) {
    return {};
}

Address IfNode::gen_ir(Ir& context) {
    
    Address end_if = context.get_label();
    Address cond = condition->gen_ir(context);
    
    context.emit_if_false(cond, end_if);

    if_body->gen_ir(context);

    context.emitLabel(end_if);

    return {};
}

Address WhileNode::gen_ir(Ir& context) {
   
    Address start_while = context.get_label();
    context.emitLabel(start_while);
    
    Address cond = condition->gen_ir(context);
    Address end_while = context.get_label();

    context.emit_if_false(cond, end_while);
    
    while_body->gen_ir(context);

    context.emitGoto(start_while);
    context.emitLabel(end_while);

    return {};
}

Address ForNode::gen_ir(Ir& context) {

    init.value()->gen_ir(context);

    Address start_for = context.get_label();
    context.emitLabel(start_for);

    //if cond false, exit for loop
    Address cond_addr = condition.value()->gen_ir(context);
    Address end_for = context.get_label();

    context.emit_if_false(cond_addr, end_for);

    for_body->gen_ir(context);

    incr.value()->gen_ir(context);

    context.emitGoto(start_for);

    context.emitLabel(end_for);

    return {};
}

Address RangeNode::gen_ir(Ir& context) {

    Address start_range = context.get_label();
    context.emitLabel(start_range);

    Address end_range = context.get_label();
    Address cond = condition->gen_ir(context);

    context.emit_if_false(cond, end_range);

    range_body->gen_ir(context);

    context.emitGoto(start_range);
    context.emitLabel(end_range);

    return {};
}

Address UnaryIncrNode::gen_ir(Ir& context) {
    return {ADDR_TYPE::VARIABLE, op, op};
}

Address BinaryExpression::gen_ir(Ir& context) {

    Address left_addr = left->gen_ir(context);
    Address right_addr = right->gen_ir(context);
    OPCODE opcode = get_opcode(op); 
    
    Address temp = context.get_temp();

    context.emit(temp, left_addr, opcode, right_addr);

    return temp;
}

Address IntegerCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value }; 
}

Address DoubleCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value }; 
}

Address BoolCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value }; 
}

Address StringCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value }; 
}

Address CharCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value }; 
}

Address IdentifierCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::VARIABLE, token.value, token.value }; 
}

Address ReturnNode::gen_ir(Ir& context) {
    Address r = ret->gen_ir(context);

    context.emit(Address{}, Address{}, OPCODE::RETURN, r);
    
    return r;
}

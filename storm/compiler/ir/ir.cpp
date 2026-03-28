#include "ir.h"
#include "../parser/node.h"
#include "address.h"
#include <algorithm>

Address Ir::gen_ir(Ir& context) {
    if(master_node) {
        return master_node->gen_ir(context);
    }

    return {};
}

static std::string opToString(OPCODE op) {
    
    switch (op) {
   
        case OPCODE::ADD:      return "ADD";
        case OPCODE::MINUS:    return "SUB";
        case OPCODE::MUL:      return "MUL";
        case OPCODE::DIV:      return "DIV";
        case OPCODE::LT:       return "LT";
        case OPCODE::GT:       return "GT";
        case OPCODE::LOE:      return "LE";
        case OPCODE::GOE:      return "GE";
        case OPCODE::RETURN:   return "RET";
        case OPCODE::GOTO:     return "GOTO";
        case OPCODE::IF_FALSE: return "IF_FALSE";
        case OPCODE::ASSIGN:   return "ASSIGN";
        case OPCODE::LABEL:    return "LABEL";
        case OPCODE::CALL:     return "CALL";
        case OPCODE::PARAM:    return "PARAM";
        case OPCODE::ARG:      return "ARG";
        case OPCODE::STORM_DEF:return "STORM_DEF";
        case OPCODE::STORM_INIT:return "STORM_INIT";
        default:               return "UNKNOWN";
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
        } else if (inst.op == OPCODE::STORM_DEF) {
            std::cout << "STRUCT_DEF " << inst.result.name;
        } else if (inst.op == OPCODE::STORM_INIT) {
            std::cout << "STORM_INIT " << inst.result.name << " " << inst.left_operand.name;
        } else if (inst.op == OPCODE::RETURN) {
            std::cout << "RETURN " << inst.right_operand.name;
        } else {
            // math logic: res = L op R
            std::cout << inst.result.name << " = " << inst.left_operand.name 
                      << " " << opToString(inst.op) << " " << inst.right_operand.name;
        }
        std::cout << "\n";
    }
    std::cout << "--------------------\n" << "\n";
}

Address MainNode::gen_ir(Ir& context) {
    for(auto& node : globals) {
        if(node) node->gen_ir(context);
    }
    context.remove_unused_variables();

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
        Address var(ADDR_TYPE::VARIABLE, name , name, type.has_value() ? type.value() : in.data_type, this->saved_offset);
        
        std::cerr << name << name << name;
        //turns x *= 5 into x = x * 5
        if(op.has_value() && op.value() != "=") {
            std::string& o = this->op.value();

            if(o == "*=") {
                context.emit(var, var, OPCODE::MUL, in);
            }else if(o == "+=") {
                context.emit(var, var, OPCODE::ADD, in);
            }else if(o == "/=") {
                context.emit(var, var, OPCODE::DIV, in);
            }else if(o == "-=") {
                context.emit(var, var, OPCODE::MINUS, in);
            }

        } else {
            // x = 5; or a = b;
            if(!context.instructions.empty() && dynamic_cast<ProcCallNode*>(init.get())) {
                Instruction& last = context.instructions.back();
                last.result = var;
            }else {
                context.emit(var, in, OPCODE::ASSIGN, Address{});
            }
        }
    } else if (!storm_init_fields.empty()) {
        Address base_var(ADDR_TYPE::VARIABLE, name, name);
        if (type.has_value()) {
            Address type_addr(ADDR_TYPE::VARIABLE, type.value(), type.value());
            context.instructions.push_back({base_var, type_addr, OPCODE::STORM_INIT, Address{}});
        }

        for (int i = 0; i < storm_init_fields.size(); i++) {
            Address val = storm_init_fields[i]->gen_ir(context); 
            std::string hidden_field = name + ".field_" + std::to_string(i);
            // fields are at saved_offset, saved_offset-8, saved_offset-16...
            Address field(ADDR_TYPE::VARIABLE, hidden_field, hidden_field, "", this->saved_offset - (i * 8));
            context.instructions.push_back({field, val, OPCODE::ASSIGN, Address{}});
        }
    }

    return {};
}

Address ProcedureNode::gen_ir(Ir& context) {

    //keeping track of current proc name for TCO
    context.curr_proc = proc_name;

    Address start_proc(ADDR_TYPE::VARIABLE, proc_name, proc_name);
    context.emitLabel(start_proc);

    context.curr_params.clear();

    for(const auto& param : parameters) {
        Address p_addr(ADDR_TYPE::PARAM, param->name, param->name, param->type.value(), param->saved_offset);
        context.curr_params.push_back(p_addr);
        context.emit(p_addr, p_addr, OPCODE::PARAM, p_addr);
    }

    body_node->gen_ir(context);

    // ensure function always returns (especially for main)
    context.emit(Address{}, Address{}, OPCODE::RETURN, Address{});

    return {};
}

Address ProcCallNode::gen_ir(Ir& context) {

    std::vector<Address> calculated_args;
    
    // convert to printf
    if (proc_name == "echo" || proc_name == "print") {
        std::string format = "`";
        for (const auto& arg : arguments) {
            if (!arg) continue;
            std::string type = arg->getType();
            if (type == "int" || type == "bool") format += "%i ";
            else if (type == "double") format += "%f ";
            else if (type == "string") format += "%s ";
            else if (type == "char") format += "%c ";
            else format += "%i ";
        }
        if (format.length() > 1) {
            format.pop_back(); // remove trailing space
        }
        if (proc_name == "echo") format += "\\n";
        format += "`";
        
        Address fmt_str(ADDR_TYPE::CONSTANT, format, format, "string");
        calculated_args.push_back(fmt_str);
    }

    for(const auto& args : arguments) {
        Address arg = args->gen_ir(context);        
        calculated_args.push_back(arg);
    }

    for(const auto& arg : calculated_args) {
        context.emit(arg, arg, OPCODE::ARG, arg);
    }   

    Address res_proc = Address{};

    if(actual_type != "void") {
        res_proc = context.get_temp(actual_type);
    }

    std::string call_name = (proc_name == "echo" || proc_name == "print") ? "printf" : proc_name;
    Address proc_label(ADDR_TYPE::VARIABLE, call_name, call_name);

    // e.g. call printf
    context.emit(res_proc, proc_label, OPCODE::CALL, Address{});


    return res_proc;
}

Address StormNode::gen_ir(Ir& context) {
    
    Address storm_label(ADDR_TYPE::VARIABLE, storm_name, storm_name);
    
    context.instructions.push_back({storm_label, Address{}, OPCODE::STORM_DEF, Address{}});

    for(const auto& s : storm_statements) {
        if(s) s->gen_ir(context);
    }

    return {};
}

Address StormDotNode::gen_ir(Ir& context) {

    if(init) {

        Address val = init->gen_ir(context);

        std::string storm_field = left + "." + right;

        Address field(ADDR_TYPE::VARIABLE, storm_field, storm_field);
        
        context.instructions.push_back({field, val, OPCODE::ASSIGN, Address{}});
    }
    
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
    std::cout << "\nENTERED FOR NODE\n\n";
    if(init.has_value()) init.value()->gen_ir(context);

    //check if binary expression for potential loop unrolling
    if(auto bin = dynamic_cast<BinaryExpression*>(condition.value().get())) {
        
        if(init && init.has_value()) {            
            auto right = dynamic_cast<IntegerCondition*>(bin->right.get());
            auto left = dynamic_cast<IntegerCondition*>(bin->left.get());
            // if both sides are constant (control is decided by user logic)
            if((left || right) && !(left && right)) {
                //no unrolling with loops over 500
                if(!(left && std::stoi(left->get_value()) > 500 || right && std::stoi(right->get_value()) > 500)) {
                    context.unroll(context, *this); 
                    return {};
                }
            }
        }
    }

    Address start_for = context.get_label();
    context.emitLabel(start_for);

    //if cond false, exit for loop
    Address cond_addr = condition.value()->gen_ir(context);

    std::cout << "-========================--=====FOR LOOP CONDITION===========================: " << cond_addr.value << ":" << cond_addr.name << "\n";
    Address end_for = context.get_label();

    context.emit_if_false(cond_addr, end_for);

    for_body->gen_ir(context);

    incr.value()->gen_ir(context);

    context.emitGoto(start_for);

    context.emitLabel(end_for);

    return {};
}

void Ir::unroll(Ir& context, ForNode& for_node) {

    auto init = for_node.init->get();
    //static casting is safe as type is known
    auto bin = static_cast<BinaryExpression*>(for_node.condition.value().get());
    
    auto left = dynamic_cast<IntegerCondition*>(bin->left.get());
    auto right = dynamic_cast<IntegerCondition*>(bin->right.get());

    int n = left ? std::stoi(left->token.value) : std::stoi(right->token.value);
    
    

    int start {0};
    auto init_var = dynamic_cast<VariableNode*>(for_node.init.value().get());

    
    
    if(init_var && init_var->init) {
        if(auto i = dynamic_cast<IntegerCondition*>(init_var->init.get())) {
            start = std::stoi(i->token.value);
        }
    }

    int number_of_tasks_per_iteration = (n > 250) ? 8 : 4;

    int n_instr = (n - start) / number_of_tasks_per_iteration;
    int remaining_instr = (n - start) % number_of_tasks_per_iteration;

    //needed so loop doesnt execute n + remaining instructions
    if(left) {
        left->token.value = std::to_string(n - remaining_instr);
    }
    if(right) {
        right->token.value = std::to_string(n - remaining_instr);
    }
    
    Address start_for = context.get_label();
    context.emitLabel(start_for);
    Address end_for = context.get_label();

    context.emit_if_false(for_node.condition.value()->gen_ir(context), end_for);

    for(int i = 0;i < number_of_tasks_per_iteration;i++) {
        //body then increment
        for_node.for_body->gen_ir(context);
        for_node.incr.value()->gen_ir(context);
    }

    context.emitGoto(start_for);
    context.emitLabel(end_for);

    if(remaining_instr > 0) {
        for(int rem = 0; rem < remaining_instr;rem++){     
            for_node.for_body->gen_ir(context);
            for_node.incr.value()->gen_ir(context);
        }
    } 
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
    Address var(ADDR_TYPE::VARIABLE, name, name, "int", this->saved_offset);
    Address constant(ADDR_TYPE::CONSTANT, "1", "1", "int");

    OPCODE op_code = (op == "++") ? OPCODE::ADD : OPCODE::MINUS;

    context.emit(var, var, op_code, constant);

    return var;
}

Address BinaryExpression::gen_ir(Ir& context) {

    Address left_addr = left->gen_ir(context);
    Address right_addr = right->gen_ir(context);
    OPCODE opcode = get_opcode(op); 
    
    Address temp = context.get_temp(left_addr.data_type);

    context.emit(temp, left_addr, opcode, right_addr);

    return temp;
}

Address IntegerCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value, "int" }; 
}

Address DoubleCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value, "double" }; 
}

Address BoolCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value, "bool" }; 
}

Address StringCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value, "string" }; 
}

Address CharCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::CONSTANT, token.value, token.value, "char" }; 
}

Address IdentifierCondition::gen_ir(Ir& context) {
    return { ADDR_TYPE::VARIABLE, token.value, token.value, actual_type, this->saved_offset }; 
}

Address ReturnNode::gen_ir(Ir& context) {
    Address r = ret->gen_ir(context);

    //tco optimization, function calls itself
    if(context.instructions.back().op == OPCODE::CALL && context.instructions.back().left_operand.name == context.curr_proc) {
        context.tco(context);
        return r;
    }

    context.emit(Address{}, Address{}, OPCODE::RETURN, r);
    
    return r;
}

void Ir::tco(Ir& context) {
    
    //pops the CALL instruction
    context.instructions.pop_back();

    std::vector<Address> arg_values;
    size_t num_params = context.curr_params.size();
    
    //extract and remove the trailing ARG instructions
    for (size_t i = 0; i < num_params; ++i) {
        if (!context.instructions.empty() && context.instructions.back().op == OPCODE::ARG) {
            arg_values.push_back(context.instructions.back().left_operand);
            context.instructions.pop_back();
        }
    }

    //reverse to match param order
    std::reverse(arg_values.begin(), arg_values.end());

    //stage arg values into temps to prevent swap problem
    std::vector<Address> temps;
    for (const auto& arg : arg_values) {
        Address t = context.get_temp(arg.data_type);
        context.emit(t, arg, OPCODE::ASSIGN, Address{});
        temps.push_back(t);
    }

    //assign params there values, prevents values being overwritten
    for (size_t i = 0; i < temps.size(); ++i) {
        context.emit(context.curr_params[i], temps[i], OPCODE::ASSIGN, Address{});
    }

    //jump back to the start of the current function
    context.emit(Address{}, Address{}, OPCODE::GOTO, Address{ADDR_TYPE::CONSTANT, context.curr_proc, context.curr_proc });
}

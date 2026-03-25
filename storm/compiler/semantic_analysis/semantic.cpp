#include "../parser/node.h"
#include "semantic.h"
#include "../parser/condition.h"
#include "conversion.h"
#include <stdexcept>
#include <string>
#include <set>

void ProcedureNode::analyze(SymbolTable* table, int& curr) {
    
    std::cerr << "=====================================\n";

    if(proc_name == "echo") {
        throw std::runtime_error("echo is a reserved keyword\n");
    }

    SymbolEntry* proc_entry = table->lookup(proc_name);
    if(!proc_entry) {
        throw std::runtime_error("Proc was not registered: " + proc_name);
    }
    
    SymbolTable proc_scope(proc_name);
    proc_scope.parent = table;

    int local_offset = 0;  
    std::set<std::string> unique_params;

    // parameters (negative offsets)
    for(const auto& var : parameters) {
        local_offset -= 8;

        if(unique_params.find(var->name) != unique_params.end()) {
            throw std::runtime_error("\nError: Parameters in function " + this->proc_name + " must have unique names\n");
        }

        unique_params.insert(var->name);
        
        SymbolEntry v(var->name, var->type.value(), local_offset, false, 0);
        proc_scope.insert(var->name, v);
    }

    body_node->analyze(&proc_scope, local_offset);

    proc_entry->stack_frame_size = local_offset;
    //full stack fram size including params
    this->stack_frame_size = local_offset;

    std::cerr << "\nProc added: " << proc_name << "\n";

}

void VariableNode::analyze(SymbolTable* table, int& curr) {

    if(init) {

        init->analyze(table, curr);
            
        std::string right_type = init->getType();

        std::cerr << "Right-type: " << right_type;

        if(type.has_value()) {

            if(type.value() == "int" && right_type == "double") {
                this->init = std::make_unique<DoubleToIntegerNode>(std::move(this->init));
                right_type = "int";
            } else if (type.value() == "double" && right_type == "int") {
                this->init = std::make_unique<IntegerToDoubleNode>(std::move(this->init));
                right_type = "double";
            }

            //this checks for i: int = "string" which would be invalid
            if(type.value() != right_type) {
                throw std::runtime_error("\n\n\nError: Variable must be initialized to the same type\n"
                    + type.value() + " : " + right_type + "\n\n");
            }
        }
    }


    if(type.has_value()) {

        //check globally for redefintion
        SymbolEntry* found = table->lookup(this->name);
        
        if(found) throw std::runtime_error("Redefintion of variable: " + name);

        if(init && type.value() != init->getType()) {
            throw std::runtime_error("\n\nError: Type mismatch: Cannot initialize" + name + "(" + type.value() + ") with " + init->getType());
        }

        std::cerr << "\n\nvanalyze:" << type.value() << ":" << name << "\n\n";
        curr -= 8;
        
        this->saved_offset = curr;
        
        SymbolEntry entry(name, type.value(), curr, false, 0);
        table->insert(name, entry);
        

    }else {

        if(!init) {
            throw std::runtime_error("\nError: No assignment on variable: " + this->name + "\n\n");
        }

        SymbolEntry* found = table->lookup(this->name);

        if(!found) { 
            throw std::runtime_error("Undefined variable: " + name + " found");
        }
        //type checking for x = 50;
        std::string right_type = init->getType();
        if(found->type == "int" && right_type == "double") {
            this->init = std::make_unique<DoubleToIntegerNode>(std::move(this->init));
            right_type = "int";
        } else if (found->type == "double" && right_type == "int") {
            this->init = std::make_unique<IntegerToDoubleNode>(std::move(this->init));
            right_type = "double";
        }

        if(found->type != right_type) {
            throw std::runtime_error("Variable must be initialized to the same type it was declared as " + found->type + " : " + right_type + "\n\n");
        }    

        this->saved_offset = found->offset;
    }

}

void IdentifierCondition::analyze(SymbolTable* table, int& curr) {

    SymbolEntry* entry = table->lookup(token.value);
    
    std::string msg("Variable " + token.value + " at line: " + std::to_string(token.line) + " col: " + std::to_string(token.col) + " was never defined\n");

    if(!entry) {
        throw std::runtime_error(msg);
    }

    //useful to find location of variable defition
    this->saved_offset = entry->offset;
    this->actual_type = entry->type;

}

void BinaryExpression::analyze(SymbolTable* table, int& curr) {
    
    if (this->left) this->left->analyze(table, curr);
    if (this->right) this->right->analyze(table, curr);

    std::string left_type = this->left->getType();
    std::string right_type = this->right->getType();

    if(left_type == "string" || right_type == "string"
        || left_type == "bool" || right_type == "bool") {
    
        if(op == "+" || op == "-" || op == "*" || op == "/") {
            throw std::runtime_error("Invalid math operation on type: " + left_type);
        }
    }

}

void MainNode::analyze(SymbolTable* table, int& current_offset) {
   
    //load all functins into symbol table
    for (const auto& node : globals) {
        if (auto proc = dynamic_cast<ProcedureNode*>(node.get())) {
            SymbolEntry entry(proc->proc_name, proc->return_type, 0, true, proc->parameters.size());
            table->insert(proc->proc_name, entry);
        }
    }

    if(table->table.empty()) {
        throw std::runtime_error("\nError: No procedure was defined\n");
    }

    //semantic analyse
    for (const auto& node : globals) {
        if (node) node->analyze(table, current_offset);
    }
}

void BodyNode::analyze(SymbolTable* table, int& current_offset) {
    //analyse nodes
    for (const auto& node : statements) {
        if (node) node->analyze(table, current_offset);
    }
}

void IfNode::analyze(SymbolTable* table, int& current_offset) {
    if (condition) condition->analyze(table, current_offset);
    if (if_body) if_body->analyze(table, current_offset);
}

void WhileNode::analyze(SymbolTable* table, int& current_offset) {
    if (condition) condition->analyze(table, current_offset);
    if (while_body) while_body->analyze(table, current_offset);
}

void ForNode::analyze(SymbolTable* table, int& current_offset) {
    if (init && init.value()) init.value()->analyze(table, current_offset);
    if (condition && condition.value()) condition.value()->analyze(table, current_offset);
    if (incr && incr.value()) incr.value()->analyze(table, current_offset);
    if (for_body) for_body->analyze(table, current_offset);
}

void RangeNode::analyze(SymbolTable* table, int& current_offset) {
    //range(i = 0..50) {}

    //check if i is already defined
    SymbolEntry* found = table->lookup(this->name);
    if (found) {
        throw std::runtime_error("Redefinition of loop iterator '" + name + "' found!");
    }

    // offset for iterator
    current_offset -= 8;
    this->saved_offset = current_offset;
    SymbolEntry entry(name, "int", current_offset, false, 0);
    table->insert(name, entry);

    if (condition) condition->analyze(table, current_offset);
    if (end_val) end_val->analyze(table, current_offset);
    if (range_body) range_body->analyze(table, current_offset);
}

void UnaryIncrNode::analyze(SymbolTable* table, int& current_offset) {
    // variable must exist to be incremented
    SymbolEntry* found = table->lookup(name);
    if (!found) {
        throw std::runtime_error("Cannot increment undefined variable '" + name + "'!");
    }
    this->saved_offset = found->offset;
}

void ProcCallNode::analyze(SymbolTable* table, int& current_offset) {
    
    //built in function so we skip lookup and error handling
    if (proc_name == "echo") {
        for (const auto& arg : arguments) {
            if (arg) arg->analyze(table, current_offset);
        }
        return;
    }

    SymbolEntry* found = table->lookup(proc_name);
    if (!found) {
        throw std::runtime_error("Calling undefined function: " + proc_name + "\n");
    }

    //get the procedures type
    std::cerr << "\nProc: " << proc_name << " with type: " << found->type << "\n";
    this->actual_type = found->type;

    if(found->param_counter != arguments.size()) {
        throw std::runtime_error("\nNumber of arguments in `" + proc_name + "` must match number of argument in proc call\n\n");
    }

    for (const auto& arg : arguments) {
        if (arg) arg->analyze(table, current_offset);
    }
    
}

void StormNode::analyze(SymbolTable* table, int& current_offset) {
    if (table->lookup(storm_name)) {
        throw std::runtime_error("Redefinition of storm (struct): " + storm_name);
    }
    SymbolEntry entry(storm_name, "storm", 0, false, 0);
    table->insert(storm_name, entry);
    for (const auto& stmt : storm_statements) {
        if (stmt) stmt->analyze(table, current_offset);
    }
}

//empty
void IntegerCondition::analyze(SymbolTable* table, int& curr) {

}

void StringCondition::analyze(SymbolTable* table, int& curr) {

}

void DoubleCondition::analyze(SymbolTable* table, int& curr) {

}

void CharCondition::analyze(SymbolTable* table, int& curr) {
    
}

void BoolCondition::analyze(SymbolTable* table, int& curr) {
    
}

void ReturnNode::analyze(SymbolTable* table, int& curr) {   
    if (ret) ret->analyze(table, curr);
}

void IntegerToDoubleNode::analyze(SymbolTable* table, int& current_offset) {
    if (node) node->analyze(table, current_offset);
}

void DoubleToIntegerNode::analyze(SymbolTable* table, int& current_offset) {
    if (node) node->analyze(table, current_offset);
}



#include "../parser/node.h"
#include "semantic.h"
#include "../parser/condition.h"
#include <stdexcept>
#include <string>


void ProcedureNode::analyze(SymbolTable* table, int& curr) {
    
    std::cerr << "=====================================\n";
    int param_offset = 16;

    if(proc_name == "echo") {
        throw std::runtime_error("echo is a reserved keyword\n");
    }

    SymbolTable proc(proc_name);
    proc.parent = table;

    //check parent table, should be the global table too
    bool found = table->lookup(proc_name);

    if(found) {
        throw std::runtime_error("Redefintion of proc found: " + proc_name);
    }

    SymbolEntry entry;
    entry.name = proc_name;
    entry.type = return_type;
    entry.is_function = true;
    
    for(const auto& var : parameters) {
        SymbolEntry v(var->name, var->type.value(), param_offset, false);
        proc.insert(var->name, v);
        param_offset += 8;
    }

    int local_offset = 0;  

    entry.stack_frame_size = local_offset;

    table->insert(proc_name, entry);

    body_node->analyze(&proc, local_offset);
    
    entry.stack_frame_size = local_offset;

    table->insert(proc_name, entry);
    std::cerr << "\nProc added: " << proc_name << "\n";

}

void VariableNode::analyze(SymbolTable* table, int& curr) {

    if(init) {
        init->analyze(table, curr);
    }

    if(type.has_value()) {
        //check globally for redefintion
        SymbolEntry* found = table->lookup(this->name);

        if(found) {
            throw std::runtime_error("Redefintion of variable: " + name);
        }else {
            curr -= 8;
            SymbolEntry entry(name, type.value(), curr, false);
            table->insert(name, entry);
        }

    }else {
        SymbolEntry* found = table->lookup(this->name);

        if(!found) { 
            throw std::runtime_error("No definition for variable: " + name + " found");
        }
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

}

void BinaryExpression::analyze(SymbolTable* table, int& curr) {
    
    std::string left_type = this->left->getType();
    std::string right_type = this->right->getType();
    
    if(left_type == "string" || right_type == "string"
        || left_type == "bool" || right_type == "bool") {
    
        if(op == "+" || op == "-" || op == "*" || op == "/") {
            throw std::runtime_error("Invalid math operation on type: " + left_type);
        }
    }
    
    this->left->analyze(table, curr);
    this->right->analyze(table, curr);
}

void MainNode::analyze(SymbolTable* table, int& current_offset) {
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
    SymbolEntry entry(name, "int", current_offset, false);
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
        throw std::runtime_error("Calling undefined function: " + proc_name);
    }

    for (const auto& arg : arguments) {
        if (arg) arg->analyze(table, current_offset);
    }
}

void StormNode::analyze(SymbolTable* table, int& current_offset) {
    if (table->lookup(storm_name)) {
        throw std::runtime_error("Redefinition of storm (struct): " + storm_name);
    }
    SymbolEntry entry(storm_name, "storm", 0, false);
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



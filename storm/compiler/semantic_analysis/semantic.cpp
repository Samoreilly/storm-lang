#include "../parser/node.h"
#include "semantic.h"
#include "../parser/condition.h"
#include <stdexcept>


void ProcedureNode::analyze(SymbolTable* table, int& curr) {
    
    int param_offset = 16;

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
    body_node->analyze(&proc, local_offset);

    entry.stack_frame_size = local_offset;

    table->insert(proc_name, entry);
    
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



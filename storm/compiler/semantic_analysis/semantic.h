#pragma once

#include <iostream>
#include <map>

struct SymbolEntry {
    std::string name;
    std::string type;
    int offset;
    bool is_function {false};
    
    int stack_frame_size;

    SymbolEntry();
    SymbolEntry(std::string n, std::string t, int o, bool is_f)
    : name(n), type(t), offset(o), is_function(is_f) {}

};

struct SymbolTable {
    SymbolTable* parent = nullptr;//pointer to previous scope
    std::string name;


    SymbolTable(std::string n) : name(n) {}
    //key = identifier, value = metadata of the object
    std::map<std::string, SymbolEntry> table;


    //whole chain lookup
    inline SymbolEntry* lookup(std::string name) {
        SymbolEntry* local = lookup_local(name);
        if (local) return local;

        if (parent != nullptr) return parent->lookup(name);
        return nullptr;
    }

    //local scope only lookup
    inline SymbolEntry* lookup_local(std::string name) {
        if (table.count(name)) {
            return &table[name];
        }
        return nullptr;
    }

    inline void insert(std::string n, SymbolEntry& entry) {
        table[n] = entry;
    }
};

// global symbol table
// when a function is found it points back to global symbol table
// say a if statement if found inside of the function it point back to function
// symbol table <- function scopes -< if statement scope
class Semantic {
public:

    //global
    SymbolTable* parent;

};

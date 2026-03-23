#pragma once

#include "../semantic_analysis/semantic.h"
#include "../../token.h"
#include <memory>
#include <string>
#include <iostream>

inline int astIndentLevel = 0;

inline void astPrintIndent() {
    for (int i = 0; i < astIndentLevel; ++i) {
        std::cout << "  ";
    }
}
/*
 pure virtual class that each node will inherit
*/
class Node {
public: 

    virtual ~Node() = default;
    virtual void print() const = 0;
    virtual void exec() = 0;
    virtual std::string to_c(int indent = 0) = 0;

    virtual std::string to_asm() = 0;    
    virtual void analyze(SymbolTable* table, int& current_offset) = 0;
    
};

class Condition : public Node {
public:

    virtual ~Condition() = default;
    virtual void print() const = 0;
    virtual void exec() = 0;
    virtual std::string to_c(int indent = 0) override = 0;
    virtual std::string getType() const { return "unknown"; }
    
    virtual std::string to_asm() override = 0;

    virtual void analyze(SymbolTable* table, int& current_offset) override = 0;
};

class BinaryExpression : public Condition {
public:
    
    std::unique_ptr<Condition> left;
    std::string op;
    std::unique_ptr<Condition> right;

    void print() const override {
        astPrintIndent();
        std::cout << "[BinaryExpression: " << op << "]\n";
        astIndentLevel++;
        if (left) left->print();
        if (right) right->print();
        astIndentLevel--;
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        std::string code;

        code += left->to_c(indent) + " ";
        code += op + " ";
        code += right->to_c(indent);

        return code;
    }

    std::string getType() const override {
        return left->getType(); // Assume left type for now or more complex logic
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;
};

class IntegerCondition : public Condition {
public:

    Token token;
    int saved_offset = 0;
    IntegerCondition(Token t) : token(t) {}

    void print() const override {
        astPrintIndent();
        std::cout << "Integer: " << token.value << "\n";
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        return token.value;
    }

    std::string getType() const override {
        return "int";
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;
};

class DoubleCondition : public Condition {
public:

    Token token;
    int saved_offset = 0; 
    DoubleCondition(Token t) : token(t) {}

    void print() const override {
        astPrintIndent();
        std::cout << "Double: " << token.value << "\n";
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        return token.value;
    }

    std::string getType() const override {
        return "double";
    }
    
    void analyze(SymbolTable* table, int& current_offset) override;


    std::string to_asm() override;
};

class BoolCondition : public  Condition {
public:

    Token token;
    int saved_offset = 0; 
    BoolCondition(Token t) : token(t) {}

    void print() const override {
        astPrintIndent();
        std::cout << "Bool: " << token.value << "\n";
    }

    void exec() override {}
    
    std::string to_c(int indent = 0) override {
        return token.value;
    }

    std::string getType() const override {
        return "bool";
    }
    
    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;
};

class StringCondition : public Condition {
public:

    Token token;
    int saved_offset = 0;
    StringCondition(Token t) : token(t) {}

    void print() const override {
        astPrintIndent();
        std::cout << "String: \"" << token.value << "\"\n";
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        return token.value;
    }

    std::string getType() const override {
        return "string";
    }
    
    void analyze(SymbolTable* table, int& current_offset) override;


    std::string to_asm() override;
};

class CharCondition : public Condition {
public:

    Token token;
    int saved_offset = 0;
    CharCondition(Token t) : token(t) {}
        
    void print() const override {
        astPrintIndent();
        std::cout << "Char: '" << token.value << "'\n";
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        return token.value;
    }

    std::string getType() const override {
        return "char";
    }
    
    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;
};

class IdentifierCondition : public Condition {
public:

    Token token;
    //useful to find location of variable defition
    int saved_offset = 0;
    std::string actual_type;
    IdentifierCondition(Token t) : token(t) {}

    void print() const override {
        astPrintIndent();
        std::cout << "Identifier: " << token.value << "\n";
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        return token.value;
    }

    std::string getType() const override {
        return actual_type;
    }
    
    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;
};

class ReturnNode : public Condition {
public:

    std::unique_ptr<Condition> ret;

    void print() const override {
        astPrintIndent();
        std::cout << "[Return]\n";
        astIndentLevel++;
        if (ret) ret->print();
        astIndentLevel--;
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        return "return " + ret->to_c(indent) + ";";
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;
};

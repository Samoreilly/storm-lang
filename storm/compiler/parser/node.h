#pragma once

#include "condition.h"

#include <vector>
#include <memory>
#include <string>
#include <optional>


class MainNode : public Node {
public:

    std::vector<std::unique_ptr<Node>> globals;

    void print() const override {
        astPrintIndent();
        std::cout << "[MainNode]\n";
        astIndentLevel++;
        for(const auto& node : globals) {
            if (node) node->print();
        }
        astIndentLevel--;
    }

    void exec() override {}
};

class BodyNode : public Node {
public:

    std::vector<std::unique_ptr<Node>> statements;

    void print() const override {
        astPrintIndent();
        std::cout << "[BodyNode]\n";
        astIndentLevel++;
        for(const auto& node : statements) {
            if (node) node->print();
        } 
        astIndentLevel--;
    }

    void exec() override {} 
};

class VariableNode : public Node {
public:

    std::string name;
    std::optional<std::string> type;
    std::unique_ptr<Condition> init;
    std::optional<std::string> op;

    // VariableNode(std::string name, std::optional<std::string> type) 
    // : name(std::move(name)), type(std::move(type)), init(nullptr) {}

    void print() const override {
        astPrintIndent();
        std::cout << "[Variable: " << name;
        if (type) std::cout << ", Type: " << type.value();
        if (op) std::cout << ", Op: " << op.value();
        std::cout << "]\n";
        if (init) {
            astIndentLevel++;
            init->print();
            astIndentLevel--;
        }
    }

    void exec() override {}
};

//encapsulates procedure data
class ProcedureNode : public Node {
public:

    std::string proc_name;
    std::string return_type;
    std::vector<std::unique_ptr<VariableNode>> parameters;

    std::unique_ptr<BodyNode> body_node;

    void print() const override {
        astPrintIndent();
        std::cout << "[ProcedureDef: " << proc_name << " -> " << return_type << "]\n";
        astIndentLevel++;
        astPrintIndent();
        std::cout << "Parameters:\n";
        astIndentLevel++;
        for (const auto& param : parameters) {
            if (param) param->print();
        }
        astIndentLevel--;
        if (body_node) body_node->print();
        astIndentLevel--;
    }

    void exec() override {}
};

class ProcCallNode : public Condition {
public:
    
    //proc being called
    std::string proc_name;
    std::vector<std::unique_ptr<Condition>> arguments;

    void print() const override {
        astPrintIndent();
        std::cout << "[ProcedureCall: " << proc_name << "()]\n";
        astIndentLevel++;
        for (const auto& arg : arguments) {
            if (arg) arg->print();
        }
        astIndentLevel--;
    }

    void exec() override {}
};

//same as struct in languages like C++ & C
class StormNode : public Node {
public:

    std::string storm_name;
    std::vector<std::unique_ptr<VariableNode>> storm_statements;

    void print() const override {
        astPrintIndent();
        std::cout << "[StormDef: " << storm_name << "]\n";
        astIndentLevel++;
        for (const auto& stmt : storm_statements) {
            if (stmt) stmt->print();
        }
        astIndentLevel--;
    }

    void exec() override {}
};


class IfNode : public Node {
public:

    std::unique_ptr<Condition> condition;
    std::unique_ptr<BodyNode> if_body;

    void print() const override {
        astPrintIndent();
        std::cout << "[IfStatement]\n";
        astIndentLevel++;
        if (condition) condition->print();
        if (if_body) if_body->print();
        astIndentLevel--;
    }

    void exec() override {}
};

class WhileNode : public Node {
public:

    std::unique_ptr<Condition> condition;
    std::unique_ptr<BodyNode> while_body;

    void print() const override {
        astPrintIndent();
        std::cout << "[WhileLoop]\n";
        astIndentLevel++;
        if (condition) condition->print();
        if (while_body) while_body->print();
        astIndentLevel--;
    }

    void exec() override {}

};

class ForNode : public Node {
public:

    std::optional<std::unique_ptr<VariableNode>> init;
    std::optional<std::unique_ptr<Condition>> condition;
    std::optional<std::unique_ptr<Node>> incr;

    std::unique_ptr<BodyNode> for_body;
    
    void print() const override {
        astPrintIndent();
        std::cout << "[ForLoop]\n";
        astIndentLevel++;
        if (init && init.value()) init.value()->print();
        if (condition && condition.value()) condition.value()->print();
        if (incr && incr.value()) incr.value()->print();
        if (for_body) for_body->print();
        astIndentLevel--;
    }

    void exec() override {}

};

class RangeNode : public Node {
public:

    std::string name;
    std::unique_ptr<Condition> condition;
    std::unique_ptr<Condition> end_val;
    std::unique_ptr<BodyNode> range_body;
    
    void print() const override {
        astPrintIndent();
        std::cout << "[RangeLoop: " << name << "]\n";
        astIndentLevel++;
        if (condition) condition->print();
        if (end_val) end_val->print();
        if (range_body) range_body->print();
        astIndentLevel--;
    }

    void exec() override {}

};


/*
    Used in for loop
*/
class UnaryIncrNode : public Node {
public:

    std::string name;
    std::string op;

    UnaryIncrNode(std::string n, std::string o) : name(std::move(n)), op(std::move(o)) {}

    void print() const override {
        astPrintIndent();
        std::cout << "[UnaryIncr: " << op << name << "]\n";
    }

    void exec() override {}
};


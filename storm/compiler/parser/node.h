#pragma once

#include "condition.h"

#include <vector>
#include <memory>
#include <string>
#include <optional>

class VariableNode;

class MainNode : public Node {
public:

    std::vector<std::unique_ptr<Node>> globals;

    void print() const override {
        for(const auto& node : globals) {
            node->print();
        }
    }

    void exec() override {}
};

class BodyNode : public Node {
public:

    std::vector<std::unique_ptr<Node>> statements;

    void print() const override {
        for(const auto& node : statements) {
            node->print();
        } 
    }

    void exec() override {} 
};

//encapsulates procedure data
class ProcedureNode : public Node {
public:

    std::string proc_name;
    std::string return_type;
    
    std::unique_ptr<BodyNode> body_node;

    void print() const override {
        // yet to implement
    }

    void exec() override {}
};

class ProcCallNode : public Condition {
public:
    
    //proc being called
    std::string proc_name;
    std::vector<std::unique_ptr<Condition>> arguments;

    void print() const override {

    }

    void exec() override {}
};

//same as struct in languages like C++ & C
class StormNode : public Node {
public:

    std::string storm_name;
    std::vector<std::unique_ptr<VariableNode>> storm_statements;

    void print() const override {

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

    }

    void exec() override {}
};

class IfNode : public Node {
public:

    std::unique_ptr<Condition> condition;
    std::unique_ptr<BodyNode> if_body;

    void print() const override {

    }

    void exec() override {}
};

class WhileNode : public Node {
public:

    std::unique_ptr<Condition> condition;
    std::unique_ptr<BodyNode> while_body;

    void print() const override {

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

    }

    void exec() override {}
};






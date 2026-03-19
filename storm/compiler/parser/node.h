#include "condition.h"

#include <vector>
#include <memory>
#include <string>
#include <optional>

class DeclVariable;
class InitVariable;


class MainNode : public Node {
public:

    std::vector<std::unique_ptr<Node>> globals;

    void print() const override {
        for(const auto& node : globals) {
            node->print();
        }
    }

    void exec() override;
};

class BodyNode : public Node {
public:

    std::vector<std::unique_ptr<Node>> statements;

    void print() const override {
        for(const auto& node : statements) {
            node->print();
        } 
    }

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

};

//same as struct in languages like C++ & C
class StormNode : public Node {
public:

    std::string storm_name;
    std::vector<DeclVariable> storm_statements;

};

class DeclVariable : public Node {
public:

    std::string name;
    std::string type;

};

class InitVariable : public Node {
public:

    std::string name;
    std::optional<std::string> type;// variable may already be declared so type is optional
    std::unique_ptr<Condition> init;// right side

};






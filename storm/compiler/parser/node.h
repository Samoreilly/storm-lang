#pragma once

#include "condition.h"
#include "../../transpiler/transpile.h"
#include <vector>
#include <memory>
#include <string>
#include <optional>

class MainNode : public Node {
public:

    std::vector<std::unique_ptr<Node>> globals;

    MainNode() {}

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

    std::string to_c(int indent = 0) override {
        std::string code;

        for(const auto& node : globals) {
            if(node) code += node->to_c(indent) + "\n"; 
        }

        return code;
    }

    void analyze(SymbolTable* table, int& current_offset) override;
    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

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

    std::string to_c(int indent = 0) override {
        std::string code;
        code += "{\n";        
            
        for(const auto& node : statements) {
            if(node) {
                std::string stmt = node->to_c(indent + 4);
                // ensures statements have semicolons if they aren't control blocks
                if (!stmt.empty() && stmt.back() != ';' && stmt.back() != '}') {
                    stmt += ";";
                }
                code += std::string(indent + 4, ' ') + stmt + "\n";
            }
        }

        code += std::string(indent, ' ') + "}";
        return code;
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

};

class IntegerToDoubleNode : public Condition {
public:

    std::unique_ptr<Condition> node;

    IntegerToDoubleNode(std::unique_ptr<Condition> n) : node(std::move(n)) {}

    std::string getType() const override {
        return "double";
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    void print() const override {
        astPrintIndent();
        std::cout << "[IntegerToDouble]\n";
        if (node) {
            astIndentLevel++;
            node->print();
            astIndentLevel--;
        }
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        return "(double)(" + node->to_c(indent) + ")";
    }

    std::string to_asm() override {
        std::string code;
        if (node) code += node->to_asm();

        code += "pop rax\n";
        //converts int to double
        code += "cvtsi2sd xmm0, rax\n";
        //moves quadword into rax
        code += "movq rax, xmm0\n";

        code += "push rax\n";

        return code;
    }
    Address gen_ir(Ir& context) override;
};


class DoubleToIntegerNode : public Condition {
public:

    std::unique_ptr<Condition> node;

    DoubleToIntegerNode(std::unique_ptr<Condition> n) : node(std::move(n)) {}

    std::string getType() const override {
        return "int";
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    void print() const override {
        astPrintIndent();
        std::cout << "[DoubleToInteger]\n";
        if (node) {
            astIndentLevel++;
            node->print();
            astIndentLevel--;
        }
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        return "(int)(" + node->to_c(indent) + ")";
    }

    std::string to_asm() override {
        std::string code;
        if (node) code += node->to_asm();

        code += "pop rax\n";

        code += "movq xmm0, rax\n";
        //converts double to int
        code += "cvttsd2si rax, xmm0\n";

        code += "push rax\n";

        return code;
    }
    Address gen_ir(Ir& context) override;
};


class VariableNode : public Node {
public:

    std::string name;
    std::optional<std::string> type;
    std::unique_ptr<Condition> init;
    std::optional<std::string> op;
    int saved_offset {0};
    std::vector<std::unique_ptr<Condition>> storm_init_fields;

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

    std::string to_c(int indent = 0) override {
        std::string code;

        if(type) {
            if(type.value() == "string") {
                code += "char* ";
            }else {
                code += type.value() + " ";
            }
        }
        code += name;

        if(op) code += " " + op.value() + " "; // Add the operator like += or *=
        else if(init) code += " = "; // Default to = for initializations
        
        if(init) {
            code += init->to_c(indent);
        }
            
        code += ";";
        return code;
    }
    
    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

};

//encapsulates procedure data
class ProcedureNode : public Node {
public:

    std::string proc_name;
    std::string return_type;
    std::vector<std::unique_ptr<VariableNode>> parameters;

    std::unique_ptr<BodyNode> body_node;
    int stack_frame_size {0};

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

    std::string to_c(int indent = 0) override {
        std::string code;

        code += return_type + " ";
        code += proc_name + " ";

        code += "(";

        for(const auto& node : parameters) {
            if(node) code += node->type.value() + " " + node->name + ", ";
        }

        if(!parameters.empty()) {
            code.pop_back(); // remove trailing space
            code.pop_back(); // remove trailing comma
        }
        
        code += ") ";
        code += body_node->to_c(indent);

        return code;
    }

    void analyze(SymbolTable* table, int& current_offset) override;


    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

};

class ProcCallNode : public Condition {
public:

    //proc being called
    std::string proc_name;
    std::vector<std::unique_ptr<Condition>> arguments;

    std::string actual_type = "void";

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

    std::string getType() const override {
        return actual_type;
    }

    std::string to_c(int indent = 0) override {
        std::string code;

        // Automatically track headers
        if (includes.count(proc_name)) {
            inclds.insert(includes.at(proc_name));
        } else if (proc_name == "echo" || proc_name == "print") {
            inclds.insert("#include <stdio.h>");
        }

        if(proc_name == "echo" || proc_name == "print") {
            code += "printf(";
            std::string format = "";
            for (const auto& arg : arguments) {
                if (!arg) continue;
                std::string type = arg->getType();
                if (type == "int") format += "%i ";
                else if (type == "double") format += "%f ";
                else if (type == "string") format += "%s ";
                else if (type == "char") format += "%c ";
                else if (type == "bool") format += "%i ";
                else format += "%i "; // defaults to %i for identifiers or unknown
            }
            if(!format.empty()) {
                format.pop_back(); // remove trailing space
                if(proc_name == "echo") format += "\\n";
                code += "\"" + format + "\", ";
            } else if(proc_name == "echo") {
                code += "\"\\n\", ";
            }
        } else if (proc_name == "printf") {
            code += "printf(";
        } else {
            code += proc_name + "(";
        }

        int prev_length = code.length();
        for(const auto& arg : arguments) {
            if(arg) code += arg->to_c(indent) + ", ";
        }

        if(prev_length != code.length()) {
            code.pop_back(); // remove trailing space
            code.pop_back(); // remove trailing comma
        }
        
        code += ")";
        
        return code;
    }
    
    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

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

    std::string to_c(int indent = 0) override {
        std::string code;
        code += "struct " + storm_name + " {\n";

        for(const auto& node : storm_statements) {
            if(node) {
                code += std::string(indent + 4, ' ') + node->to_c(indent + 4) + "\n";
            }
        }
            
        code += "};\n";
        
        return code;
    }
    
    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

};

class StormDotNode : public Node {
public:

    //player.damaage = 100;
    std::string left;//player
    std::string right;//damage
    std::unique_ptr<Condition> init;//100

    StormDotNode() {}
    StormDotNode(std::string l, std::string r, std::unique_ptr<Condition> i) : left(l), right(r), init(std::move(i)) {}

    void print() const override {}

    void exec() override {}

    std::string to_c(int indent = 0) override {}

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

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

    std::string to_c(int indent = 0) override {
        std::string code;

        code += "if(" + condition->to_c(indent) + ") ";
        code += if_body->to_c(indent);

        return code;
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

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

    std::string to_c(int indent = 0) override {
        std::string code;

        code += "while(" + condition->to_c(indent) + ") ";
        code += while_body->to_c(indent);
        
        return code;
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

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

    std::string to_c(int indent = 0) override {
        std::string code;
    
        code += "for(";
        
        if(init) {
            std::string init_c = init.value()->to_c(indent);
            if(init_c.back() == ';') init_c.pop_back();
            code += init_c + "; ";
        } else {
            code += "; ";
        }
        
        if(condition) {
            code += condition.value()->to_c(indent) + "; ";
        } else {
            code += "; ";
        }
        
        if(incr) {
            std::string incr_c = incr.value()->to_c(indent);
            if(incr_c.back() == ';') incr_c.pop_back();
            code += incr_c;
        }
        
        code += ") ";
        code += for_body->to_c(indent);

        return code;
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

};

class RangeNode : public Node {
public:

    std::string name;
    std::unique_ptr<Condition> condition;
    std::unique_ptr<Condition> end_val;
    std::unique_ptr<BodyNode> range_body;
    int saved_offset {0};
    
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

    //will convert range node to a while loop as C doesn't have a range loop
    std::string to_c(int indent = 0) override {
        std::string code;

        code += "int " + name + " = " + condition->to_c(indent) + ";\n"; 
        code += std::string(indent, ' ') + "while(" + name + " < " + end_val->to_c(indent) + ") ";
        
        std::string body_c = range_body->to_c(indent);
        size_t pos = body_c.rfind("}");
        
        if (pos != std::string::npos) {
            // finds the last line start to insert before the brace
            size_t start_of_brace_line = body_c.rfind("\n", pos);
            if (start_of_brace_line == std::string::npos) start_of_brace_line = 0;
            else start_of_brace_line++; // Skip the newline

            body_c.insert(start_of_brace_line, std::string(indent + 4, ' ') + name + "++;\n");
        }

        code += body_c;

        return code;
    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;

};


/*
    Used in for loop
*/
class UnaryIncrNode : public Node {
public:

    std::string name;
    std::string op;
    int saved_offset {0};

    UnaryIncrNode(std::string n, std::string o) : name(std::move(n)), op(std::move(o)) {}

    void print() const override {
        astPrintIndent();
        std::cout << "[UnaryIncr: " << op << name << "]\n";
    }

    void exec() override {}

    std::string to_c(int indent = 0) override {
        std::string code;

        code += name + op + ";";

        return code;

    }

    void analyze(SymbolTable* table, int& current_offset) override;

    std::string to_asm() override;

    Address gen_ir(Ir& context) override;


};

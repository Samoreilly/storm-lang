#pragma once

#include <string>

//unary operations will be turned to i = i + 1
enum class OPCODE {ADD, MINUS, MUL, DIV, LT, GT, LOE, GOE, RETURN, GOTO, IF_FALSE, ASSIGN, LABEL, CALL, PARAM, ARG, STORM_DEF, STORM_INIT};
enum class ADDR_TYPE {CONSTANT, TEMP, VARIABLE, PARAM, ARGS};

struct Address {
    ADDR_TYPE type;
    std::string name;
    std::string value; 
    std::string data_type;
    int offset = 0;
    
    Address(ADDR_TYPE t, std::string n, std::string v, std::string dt = "", int o = 0) 
    : type(t), name(n), value(v), data_type(dt), offset(o) {}
    Address() : type(ADDR_TYPE::CONSTANT), name(""), value(""), data_type(""), offset(0) {}
};

inline OPCODE get_opcode(const std::string& op) {
    if (op == "+") return OPCODE::ADD;
    if (op == "-") return OPCODE::MINUS;
    if (op == "*") return OPCODE::MUL;
    if (op == "/") return OPCODE::DIV;
    if (op == "=") return OPCODE::ASSIGN;
    if(op == "<")  return OPCODE::LT;
    if(op == ">")  return OPCODE::GT;
    if(op == "<=") return OPCODE::LOE;
    if(op == ">=") return OPCODE::GOE;
    return OPCODE::ASSIGN; // fallback
}



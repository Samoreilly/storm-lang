#pragma once

#include <string>

//unary operations will be turned to i = i + 1
enum class OPCODE {ADD, MINUS, MUL, DIV, LT, GT, LOE, GOE, RETURN, GOTO, IF_FALSE, ASSIGN, LABEL, CALL, PARAM, ARG};
enum class ADDR_TYPE {CONSTANT, TEMP, VARIABLE, PARAM, ARGS};

struct Address {
    std::string name;
    std::string value;
    ADDR_TYPE type;
    Address(ADDR_TYPE t, std::string n, std::string v) : type(t), name(n), value(v) {}
    Address() {}
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



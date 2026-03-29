#pragma once

#include "../ir/ir.h"
#include <optional>
#include <vector>
#include <map>
#include <string>

struct LiveInterval {
    std::string assigned_reg;//-1 means no reg assigned
    std::string var_name;
    std::string data_type; // string, double, or int
    int start {0};// when variable enters the program
    int end {0}; // leaves the program
};


class RegAlloc {

    std::vector<Instruction>& instructions;
    
    // integer/string register pool (excluding rax)
    std::vector<std::string> int_registers = {"rbx", "r12", "r13", "r14", "r15"};
    
    // float register pool (excluding xmm0)
    std::vector<std::string> double_registers = {"xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"};

    std::map<std::string, LiveInterval> map_index;

    //waiting to be handled
    std::vector<LiveInterval> unhandled;
    std::vector<LiveInterval> handled;
    
    std::vector<LiveInterval> active;
    
    //variable to assigned register
    std::map<std::string, std::string> var_reg;
   

    void update_interval(const Address& addr, int i);
    void map_to_list();

    void free_registers(LiveInterval& intrvl);
    
    //spill to stack if not available
    inline std::optional<std::string> get_reg(const std::string& type) {
        if (type == "double") {
            if (double_registers.empty()) return std::nullopt;
            std::string r = double_registers.back();
            double_registers.pop_back();
            return r;
        } else {
            if (int_registers.empty()) return std::nullopt;
            std::string r = int_registers.back();
            int_registers.pop_back();
            return r;
        }
    }

public:
 
    RegAlloc(std::vector<Instruction>& i) : instructions(i) {}
    RegAlloc();

    void build_list();
    void linear_scan();

    std::string get_reg_for(const std::string& var_name) {
        if (var_reg.count(var_name)) return var_reg.at(var_name);
        return "stack";
    }

};

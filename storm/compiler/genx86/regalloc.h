#pragma once

#include "../ir/ir.h"
#include <vector>

struct LiveInterval {
    std::string assigned_reg;//-1 means no reg assigned
    std::string var_name;
    int start {0};// when variable enters the program
    int end {0}; // leaves the program
};


class RegAlloc {

    std::vector<Instruction>& instructions;

    std::map<std::string, LiveInterval> map_index;

    std::vector<LiveInterval> unhandled;
    std::vector<LiveInterval> handled;
    
    std::vector<LiveInterval> active;
    std::vector<LiveInterval> inactive;
    
    void update_interval(const Address& addr, int i);
    void map_to_list();

public:
 
    RegAlloc(std::vector<Instruction>& i) : instructions(i) {}
    RegAlloc();

    void build_list();
    void linear_scan();

};

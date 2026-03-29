
#include "regalloc.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>





bool comp(LiveInterval& a, LiveInterval& b) {
    return a.start < b.start;
}

/* 
    LINEAR SCAN ALGORITHM consists of two register pools ( double) and other a register pool for every other datatype
    Calls free_registers to see if there is an outdated variable that has been assigned to a register, incoming intrvl takes that register
    if no register is available, we resort to stealing from a variable with the longest lifetime
    this method is called in backend.h get_addr() when finding a register for a variable 
*/

void RegAlloc::linear_scan() {
    for(int i = 0; i < unhandled.size(); i++) {
        LiveInterval& intrvl = unhandled[i]; 

        free_registers(intrvl);

        std::optional<std::string> reg = get_reg(intrvl.data_type);
        
        if(reg.has_value()) {
            intrvl.assigned_reg = reg.value();
            active.push_back(intrvl);
            var_reg[intrvl.var_name] = intrvl.assigned_reg;
        } else {
            // find the variable that life time ends last
            auto it = std::max_element(active.begin(), active.end(), 
                [](const LiveInterval& a, const LiveInterval& b) { return a.end < b.end; });

            if (it != active.end() && it->end > intrvl.end) {
                // use that register as its used last
                intrvl.assigned_reg = it->assigned_reg;
                
                // spill the variable to the stack
                var_reg[it->var_name] = "stack";

                for (auto& item : unhandled) {
                    if (item.var_name == it->var_name) {
                        item.assigned_reg = "-1";
                        break;
                    }
                }
                
                // swap them in the active list as the other variable is going to stack
                *it = intrvl; 
                var_reg[intrvl.var_name] = intrvl.assigned_reg;
            } else {
                var_reg[intrvl.var_name] = "stack";
                intrvl.assigned_reg = "-1";
            }
        }
    }
}

void RegAlloc::free_registers(LiveInterval& curr_interval) {
    auto it = std::remove_if(active.begin(), active.end(), [&](LiveInterval& intrvl) {
        if (intrvl.end < curr_interval.start) {
            // return back to reg pool
            if (intrvl.data_type == "double") {
                double_registers.push_back(intrvl.assigned_reg);
            } else {
                int_registers.push_back(intrvl.assigned_reg);
            }
            handled.push_back(std::move(intrvl));
            return true;
        }
        return false;
    });

    active.erase(it, active.end());
}


void RegAlloc::build_list() {
    int index {0};
    for(const auto& i : instructions) {
        update_interval(i.result, index);
        update_interval(i.left_operand, index);
        update_interval(i.right_operand, index);
        index++;
    }
    map_to_list();
}


void RegAlloc::update_interval(const Address& addr, int i) {
    if (addr.type == ADDR_TYPE::VARIABLE || addr.type == ADDR_TYPE::TEMP) {
        std::string name = addr.name;
        if (map_index.find(name) == map_index.end()) {
            map_index[name] = {"-1", name, addr.data_type, i, i};
        } else {
            map_index[name].end = i;
        }
    }
}

void RegAlloc::map_to_list() {
    for(auto it = map_index.begin(); it != map_index.end(); ++it) {
       unhandled.push_back(it->second); 
    }
    std::sort(unhandled.begin(), unhandled.end(), comp);
}


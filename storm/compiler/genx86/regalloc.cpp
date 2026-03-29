
#include "regalloc.h"
#include <iostream>
#include <map>

void RegAlloc::linear_scan() {

}


void RegAlloc::build_list() {

    int index {0};

    for(const auto& i : instructions) {
        update_interval(i.result, index);
        update_interval(i.left_operand, index);
        update_interval(i.right_operand, index);
        update_interval(i.result, index);
        index++;
    }

    map_to_list();
}


void RegAlloc::update_interval(const Address& addr, int i) {
    if (addr.type == ADDR_TYPE::VARIABLE || addr.type == ADDR_TYPE::TEMP) {
        std::string name = addr.name;
        //check if variable is in the map
        if (map_index.find(name) == map_index.end()) {
            map_index[name] = {"-1", name, i, i};
        } else {
            map_index[name].end = i;
        }
    }
}

bool comp(LiveInterval a, LiveInterval b) {
    return a.start < b.start;
}

void RegAlloc::map_to_list() {
   
    std::cout << "ENTERED";
    for(std::map<std::string, LiveInterval>::iterator it = map_index.begin(); it != map_index.end();++it) {
       unhandled.push_back(it->second); 
    }

    std::sort(unhandled.begin(), unhandled.end(), comp);

    for(const auto& i : unhandled) {
        std::cout << "\n" << i.start << "\n";
    }

}


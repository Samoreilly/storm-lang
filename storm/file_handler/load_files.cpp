
#include "load_files.h"
#include <iostream>
#include <fstream>

const std::string Load_Files::load_file() {

    std::string file_name = "storm-example.storm";
    std::ifstream file(file_name);

    std::string file_content;

    if(!file.is_open()) {
        std::cerr << "Failed to open file:" << file_name << "\n";
    }

    std::string line;
    while(std::getline(file, line)) {
        file_content += (line + "\n");
    }
    
    file.close();

    std::cout << "\nFile Content:\n" << file_content << "\n";

    return file_content;
}


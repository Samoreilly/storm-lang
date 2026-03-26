
#include "load_files.h"
#include <filesystem>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <fstream>

std::vector<std::string> Load_Files::load_files(char* argv[], int size) {

    std::vector<std::string> file_contents;
    int len = 0;

    for(int i = 1;i < size;i++) {
        char* file_name = argv[i];
        file_contents.push_back(get_file_content(std::string(file_name)));
    }

    return file_contents;
}

std::vector<std::string> Load_Files::load_folder() {

    std::vector<std::string> file_contents;

    std::filesystem::path curr = std::filesystem::current_path();

    std::cerr << curr.string();

    for(const auto& f : std::filesystem::directory_iterator(curr)) {
        
        if(f.is_regular_file() && f.path().extension() == ".storm") {
            file_contents.push_back(get_file_content(f.path().filename().string()));
        }
    }

    return file_contents;

}

std::string Load_Files::get_file_content(std::string file_name) {
   
    std::cerr << file_name << "\n";

    std::ifstream file(file_name);

    if(!file.is_open()) {
        throw std::runtime_error("File " + file_name + " was not found");
    }

    std::string line, file_content;
    while(std::getline(file, line)) {
        file_content += (line + "\n");
    }
    
    file.close();

    return file_content;

}

void Load_Files::write_to_file(std::string code) {

    std::cerr << "try";
    std::ofstream c_file("transpiled.c");

    c_file << code;

    c_file.close();

}


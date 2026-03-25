#pragma once

#include <iostream>
#include <string>
#include <vector>

class Load_Files {

public:

    std::vector<std::string> load_files(char* argv[], int size);
    std::string get_file_content(std::string file_content);
    std::string load_folder();
    void write_to_file(std::string code);

};


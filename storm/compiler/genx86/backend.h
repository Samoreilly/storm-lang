#include <iostream>



static std::string data = "section .data \n\t";

// double and are string are too large so we store them in section .data
// with unique names
static int string_counter;
static int double_counter;
static int if_counter;//to have unique subroutine names for jumps
static std::string current_exit_label = "";

class Backend {

public:

};

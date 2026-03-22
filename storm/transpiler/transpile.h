#pragma once
#include <memory>
#include <unordered_map>
#include <set>
#include <string>

class MainNode;

// Standalone variables for include tracking
inline std::set<std::string> inclds;

inline const std::unordered_map<std::string, std::string> includes = {
    {"printf", "#include <stdio.h>"},
    {"scanf", "#include <stdio.h>"},
    {"fprintf", "#include <stdio.h>"},
    {"fscanf", "#include <stdio.h>"},
    {"sprintf", "#include <stdio.h>"},
    {"snprintf", "#include <stdio.h>"},

    {"malloc", "#include <stdlib.h>"},
    {"calloc", "#include <stdlib.h>"},
    {"realloc", "#include <stdlib.h>"},
    {"free", "#include <stdlib.h>"},
    {"exit", "#include <stdlib.h>"},
    {"atoi", "#include <stdlib.h>"},
    {"atof", "#include <stdlib.h>"},

    {"strlen", "#include <string.h>"},
    {"strcpy", "#include <string.h>"},
    {"strncpy", "#include <string.h>"},
    {"strcmp", "#include <string.h>"},
    {"strncmp", "#include <string.h>"},
    {"strcat", "#include <string.h>"},
    {"strncat", "#include <string.h>"},

    {"fopen", "#include <stdio.h>"},
    {"fclose", "#include <stdio.h>"},
    {"fread", "#include <stdio.h>"},
    {"fwrite", "#include <stdio.h>"},

    {"time", "#include <time.h>"},
    {"clock", "#include <time.h>"},

    {"sqrt", "#include <math.h>"},
    {"pow", "#include <math.h>"},
    {"sin", "#include <math.h>"},
    {"cos", "#include <math.h>"},

    {"toupper", "#include <ctype.h>"},
    {"tolower", "#include <ctype.h>"},
    {"isdigit", "#include <ctype.h>"},

    {"assert", "#include <assert.h>"},

    {"rand", "#include <stdlib.h>"},
    {"srand", "#include <stdlib.h>"}
};

class Transpile {
    
    const std::unique_ptr<MainNode>& root;

public:
 
    Transpile(const std::unique_ptr<MainNode>& r) : root(r) {}
    const std::string transpile();
};

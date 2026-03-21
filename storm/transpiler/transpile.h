
#include "../compiler/parser/node.h"
#include <memory>

class Transpile {

    const std::unique_ptr<MainNode>& root;

public:

    Transpile(const std::unique_ptr<MainNode>& r) : root(std::move(r)) {
    }

    void transpile();

};

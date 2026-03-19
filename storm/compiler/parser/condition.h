
#include <memory>
#include <string>
/*
 pure virtual class that each node will inherit
*/
class Node {
public: 
    virtual ~Node() = 0;
    virtual void print() const = 0;
    virtual void exec() = 0;
};


class Condition {
public:

    virtual ~Condition() = 0;
    virtual void print() const = 0;
    virtual void exec() = 0;
};

class BinaryExpression : public Condition {
public:
    
    std::unique_ptr<Condition> left;
    std::string op;
    std::unique_ptr<Condition> right;

    void print() const override {

    }
};

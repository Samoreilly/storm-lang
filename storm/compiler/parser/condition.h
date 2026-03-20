#pragma once

#include "../../token.h"
#include <memory>
#include <string>
#include <iostream>
/*
 pure virtual class that each node will inherit
*/
class Node {
public: 
    virtual ~Node() = 0;
    virtual void print() const = 0;
    virtual void exec() = 0;
};


class Condition : public Node{
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

class IntegerCondition : public Condition {
public:

    Token token;
    IntegerCondition(Token t) : token(t) {}

    void print() const override {
        std::cout << token.value;
    }
};

class DoubleCondition : public Condition {
public:

    Token token;
    DoubleCondition(Token t) : token(t) {}
};

class BoolCondition : public  Condition {
public:

    Token token;
    BoolCondition(Token t) : token(t) {}
};

class StringCondition : public Condition {
public:

    Token token;
    StringCondition(Token t) : token(t) {}
};



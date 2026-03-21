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
    virtual ~Node() = default;
    virtual void print() const = 0;
    virtual void exec() = 0;
};

class Condition : public Node {
public:

    virtual ~Condition() = default;
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

    void exec() override {}
};

class IntegerCondition : public Condition {
public:

    Token token;
    IntegerCondition(Token t) : token(t) {}

    void print() const override {
        std::cout << token.value;
    }

    void exec() override {}
};

class DoubleCondition : public Condition {
public:

    Token token;
    DoubleCondition(Token t) : token(t) {}

    void print() const override {
        std::cout << token.value;
    }

    void exec() override {}
};

class BoolCondition : public  Condition {
public:

    Token token;
    BoolCondition(Token t) : token(t) {}

    void print() const override {
        std::cout << token.value;
    }

    void exec() override {}
};

class StringCondition : public Condition {
public:

    Token token;
    StringCondition(Token t) : token(t) {}

    void print() const override {
        std::cout << token.value;
    }

    void exec() override {}
};

class CharCondition : public Condition {
public:

    Token token;
    CharCondition(Token t) : token(t) {}

    void print() const override {
        std::cout << token.value;
    }

    void exec() override {}
};

class IdentifierCondition : public Condition {
public:

    Token token;
    IdentifierCondition(Token t) : token(t) {}

    void print() const override {
        std::cout << token.value;
    }

    void exec() override {}
};

class ReturnNode : public Condition {
    
    std::unique_ptr<Condition> ret;
};


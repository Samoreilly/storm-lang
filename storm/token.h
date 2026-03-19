#pragma once

#include <boost/container/set.hpp>
#include <string>
#include <boost/unordered_map.hpp>
#include <unordered_set>
#include <unordered_map>

enum class TokenType {

    TEMPEST,// entry point in a program 
    PROC,
    STORM,
    KEYWORD,

    FOR,
    WHILE, 
    RANGE,
    IF,

    VOID,
    INTEGER,
    DOUBLE,
    STRING,
    BOOL,
    CHAR,
    
    IDENTIFIER,
    SYMBOL,
    OPERATOR,
    UNARY_OP,

    UPTO
};

struct Token {
    TokenType type;
    std::string value;

    int line;
    int col;
};

static const bool is_symbol(char c) {
   return c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')' || c == ';' || c == ':' || c == ',' || c == '.';
}

static const bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>' || c == '!';
}

static const boost::unordered_map<std::string, TokenType> FEATURES = {
    { "proc",   TokenType::PROC    },
    { "storm",  TokenType::STORM   },
    { "for",    TokenType::FOR     },
    { "if",     TokenType::IF      },
    { "while",  TokenType::WHILE   },
    { "range",  TokenType::RANGE   },
    { "int",    TokenType::INTEGER },
    { "double", TokenType::DOUBLE  },
    { "string", TokenType::STRING  },
    { "bool",   TokenType::BOOL    },
    { "void",   TokenType::VOID    },
    { "tempest",TokenType::TEMPEST },
    {},
    {}
};

static const boost::container::set<std::string> KEYWORDS = {
    "proc", "for" 
};

static const std::unordered_map<TokenType, std::string> token_tostring = {
   
    {TokenType::TEMPEST,    "TEMPEST"},
    {TokenType::PROC,       "PROC"},
    {TokenType::KEYWORD,    "KEYWORD"},
    {TokenType::STORM,      "STORM"},
    {TokenType::FOR,        "FOR"},
    {TokenType::WHILE,      "WHILE"},
    {TokenType::RANGE,      "RANGE"},
    {TokenType::IF,         "IF"},

    {TokenType::VOID,       "VOID"},
    {TokenType::INTEGER,    "INTEGER"},
    {TokenType::DOUBLE,     "DOUBLE"},
    {TokenType::STRING,     "STRING"},
    {TokenType::BOOL,       "BOOL"},
    {TokenType::CHAR,       "CHAR"},

    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::SYMBOL,     "SYMBOL"},
    {TokenType::OPERATOR,   "OPERATOR"},
    {TokenType::UNARY_OP,   "UNARY_OP"},
    {TokenType::UPTO,       "UPTO"}
};

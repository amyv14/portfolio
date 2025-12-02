#pragma once 
#include <string>
#include <optional>
#include <vector>

enum class Tokens
{
    PRINT,
    IF,
    THEN,
    ENDIF,
    LET,
    INPUT,
    WHILE,
    REPEAT,
    ENDWHILE,
    GOTO,
    LABEL,
    INTEGER,
    IDENT,
    STRING,
    COMP,
    ASSIGN,
    NOT,
    PLUS,
    MINUS,
    DIVIDE,
    TIMES,
    SEMICOLON
};

struct Token
{
    Tokens type;
    std::optional<std::string> value;
};

std::string tokenTypeToString(Tokens type); 
std::vector<Token> tokenizer(const std::string &str);
std::ostream &operator<<(std::ostream &os, const Token &token);
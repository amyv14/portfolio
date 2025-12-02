#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <unordered_map>

//    ./lexer ../scripts/1.basic
// g++ -std=c++2a lexer.cpp -o lexer

// print out our tokens as strings
std::string tokenTypeToString(Tokens type)
{
    switch (type)
    {
    case Tokens::PRINT:return "PRINT";
    case Tokens::IF:return "IF";
    case Tokens::THEN:return "THEN";
    case Tokens::ENDIF:return "ENDIF";
    case Tokens::LET:return "LET";
    case Tokens::INPUT:return "INPUT";
    case Tokens::WHILE:return "WHILE";
    case Tokens::REPEAT:return "REPEAT";
    case Tokens::ENDWHILE:return "ENDWHILE";
    case Tokens::GOTO:return "GOTO";
    case Tokens::LABEL:return "LABEL";
    case Tokens::INTEGER:return "INTEGER";
    case Tokens::IDENT:return "IDENT";
    case Tokens::STRING:return "STRING";
    case Tokens::COMP: return "COMP";
    case Tokens::ASSIGN:return "ASSIGN";
    case Tokens::NOT:return "NOT";
    case Tokens::PLUS:return "PLUS";
    case Tokens::MINUS:return "MINUS";
    case Tokens::DIVIDE:return "DIVIDE";
    case Tokens::TIMES:return "TIMES";
    case Tokens::SEMICOLON:return "SEMICOLON";
    default:return "UNKNOWN";
    }
}

// hashmap of keywords
std::unordered_map<std::string, Tokens> keywords =
    {
        {"print", Tokens::PRINT},
        {"if", Tokens::IF},
        {"then", Tokens::THEN},
        {"endif", Tokens::ENDIF},
        {"let", Tokens::LET},
        {"input", Tokens::INPUT},
        {"while", Tokens::WHILE},
        {"repeat", Tokens::REPEAT},
        {"endwhile", Tokens::ENDWHILE},
        {"goto", Tokens::GOTO},
        {"label", Tokens::LABEL}};

// hashmap for the two char ops
std::unordered_map<std::string, Tokens> two_char =
    {
        {"==", Tokens::COMP},
        {"!=", Tokens::COMP},
        {"<=", Tokens::COMP},
        {">=", Tokens::COMP}};

// hashmap for the one char ops
std::unordered_map<std::string, Tokens> one_char =
    {
        {"=", Tokens::ASSIGN},
        {"<", Tokens::COMP},
        {">", Tokens::COMP},
        {"!", Tokens::NOT},
        {"+", Tokens::PLUS},
        {"/", Tokens::DIVIDE},
        {"-", Tokens::MINUS},
        {"*", Tokens::TIMES},
        {";", Tokens::SEMICOLON}};

// main tokenizer function that takes in file as string input
std::vector<Token> tokenizer(const std::string &str)
{
    // create vector to hold tokens
    std::vector<Token> tokens;
    std::size_t i = 0;

    // loop through each char till EOF
    while (i < str.size())
    {
        char c = str[i];

        // skip irrelevant spaces
        if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
        {
            i++;
            continue;
        }
        // skip through comments
        if (c == '#')
        {
            while (i < str.size() && str[i] != '\n')
            {
                ++i;
            }
            continue;
        }
        // handle one and two char operators
        else if ((c == ';') || (c == '<') || (c == '>') || (c == '*') || (c == '/') ||
                 (c == '+') || (c == '-') || (c == '=') || (c == '!'))
        {
            if (i + 1 < str.size())
            {
                // check if the first op is followed by a valid second op
                std::string op2{str[i], str[i + 1]};
                // if so push back both operators and update i accordingly
                if (two_char.contains(op2))
                {
                    tokens.push_back(Token{two_char.at(op2), op2});
                    i += 2;
                    continue;
                }
            }
            // else handle one char operators
            std::string op1{str[i]};
            if (one_char.contains(op1))
            {
                tokens.push_back(Token{one_char.at(op1), op1});
                ++i;
                continue;
            }
            ++i;
            continue;
        }
        // checks for letter or underscore
        else if (std::isalpha(c) || c == '_')
        {
            // advances the index while we have a letter or underscore
            std::size_t start_index = i;
            while (i < str.size() && (isalnum(str[i]) || str[i] == '_'))
            {
                i++;
            }

            // slices the wanted token use the i index
            std::string substring = str.substr(start_index, i - start_index);

            // if that word is part of our keyword hashmap
            if (keywords.contains(substring))
            {
                // add to tokens vector
                tokens.push_back(Token{keywords[substring], substring});
            }
            else
            {
                // else add it as an identifer
                tokens.push_back(Token{Tokens::IDENT, substring});
            }
            continue;
        }
        // if we have a quote
        else if (c == '"')
        {
            // mark the start index and consume the first quote
            int start_index = i;
            i++;
            // advance through till we reach the end quote
            while ((i < str.size()) && (str[i] != '"'))
            {
                i++;
            }

            // make sure we throw an error if there is no closing quote
            if (i >= str.size())
            {
                std::cerr << "Must have closing quote\n";
                std::exit(1);
            }

            // splice the substring using the i index and push it back as a string
            std::string substring = str.substr((start_index + 1), i - (start_index + 1));
            tokens.push_back(Token{Tokens::STRING, substring});

            // consumes the closing quote
            if (i < str.size() && str[i] == '"')
            {
                i++;
            }
            continue;
        }
        // if we have a digit
        else if (std::isdigit(c))
        {
            // advance i till we reach the end of the digit
            int start_index = i;
            while ((i < str.size()) && std::isdigit(str[i]))
            {
                i++;
            }
            // splice based off i index
            std::string substring = str.substr(start_index, i - start_index);
            tokens.push_back(Token{Tokens::INTEGER, substring});
        }
        // else advance
        else
        {
            i++;
        }
    }
    // finally return all our tokens
    return tokens;
}
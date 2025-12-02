#include "lexer.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <filesystem>

// g++ -std=c++2a ./cpp/compiler.cpp ./cpp/lexer.cpp -o compile
// ./compile ./cpp/example.basic

namespace fs = std::filesystem;

// create struct for emitter to keep track of headers and vars
struct Emitter
{
    std::vector<std::string> headers;
    std::unordered_map<std::string, std::string> symbols;
    std::string code;

    // helper functions that add lines and write to file
    void Add(const std::string &s);
    void AddLine(const std::string &s);
    void AddHeader(const std::string &h);
    std::string ToString();
    void WriteToFile(const std::string &filename);
};

// simply adds to given string
void Emitter::Add(const std::string &s)
{
    code += s;
}
// adds line to given string
void Emitter::AddLine(const std::string &s)
{
    code += s + "\n";
}

// adds header to header vector
void Emitter::AddHeader(const std::string &h)
{
    headers.push_back(h);
}

// constructs final c code
std::string Emitter::ToString()
{
    std::string final_string;
    // starts with adding headers
    for (size_t i = 0; i < headers.size(); i++)
        final_string += headers[i] + "\n";
    // follows with the code
    final_string += code;
    return final_string;
}

// writes to file
void Emitter::WriteToFile(const std::string &filename)
{
    std::ofstream output(filename);
    // check if file can be opened
    if (!output.is_open())
    {
        std::cerr << "Failed to open output file: " << filename << "\n";
        return;
    }
    output << ToString();
    output.close();
}

struct Parser
{
    // create vector for token, checking track of current
    const std::vector<Token> &tokens;
    size_t current = 0;

    // create instance of emmiter struct
    Emitter &emitter;

    Parser(const std::vector<Token> &t, Emitter &e) : tokens(t), emitter(e) {}

    // checks if we reached the end
    bool atend() const
    {
        return current >= tokens.size();
    }
    // looks at current token
    const Token &peektoken() const
    {
        return tokens[current];
    }
    // advances to next token and returns previous
    const Token &getnexttoken()
    {
        if (!atend())
        {
            current++;
        }
        return tokens[current - 1];
    }

    // returns the last token
    const Token &last() const
    {
        return tokens[current - 1];
    }

    // boolean to check the type of a given token
    bool checktype(Tokens type) const
    {
        return (!atend() && tokens[current].type == type);
    }

    // takes in a token and checks it against an expected token type, advancing past if equal
    void expect(Tokens type, const std::string &expected)
    {
        if (checktype(type))
        {
            getnexttoken();
            return;
        }
        if (atend())
            std::cerr << "parser expects: " << expected << " but got EOF\n";
        else
            std::cerr << "parser expects: " << expected
                      << " but got " << tokenTypeToString(peektoken().type) << "\n";
        std::exit(1);
    }

    // helper to check for semicolon
    void semicolon()
    {
        expect(Tokens::SEMICOLON, "semicolon");
    }

    // handles comparisons
    std::string comparison()
    {
        // expression first on the left
        std::string left = expression();
        bool seen = false;
        // construct based off both expressions and comparison operator
        while (checktype(Tokens::COMP))
        {
            seen = true;
            std::string op = peektoken().value.value();
            getnexttoken();
            left = "(" + left + " " + op + " " + expression() + ")";
        }
        // return error if comparison not found
        if (!seen)
        {
            std::cerr << "parser expects: comparison expression\n";
            std::exit(1);
        }
        return left;
    }

    std::string expression()
    {
        // start with a term
        std::string left = term();
        // construct based off both terms and unary comparison
        while (checktype(Tokens::PLUS) || checktype(Tokens::MINUS))
        {
            std::string op = peektoken().value.value();
            getnexttoken();
            left = "(" + left + " " + op + " " + term() + ")";
        }
        return left;
    }

    std::string term()
    {
        // start with unary
        std::string left = unary();
        // construct based off both unaries and comparison operator
        while (checktype(Tokens::TIMES) || checktype(Tokens::DIVIDE))
        {
            std::string op = peektoken().value.value();
            getnexttoken();
            left = "(" + left + " " + op + " " + unary() + ")";
        }
        return left;
    }

    std::string unary()
    {
        // checks for operator
        if (checktype(Tokens::PLUS) || checktype(Tokens::MINUS) || checktype(Tokens::NOT))
        {
            std::string op = peektoken().value.value_or("!");
            getnexttoken();
            return "(" + op + unary() + ")";
        }
        return primary();
    }

    std::string primary()
    {
        // advance while there is an integer or identifier
        if (checktype(Tokens::INTEGER) || checktype(Tokens::IDENT))
        {
            std::string v = peektoken().value.value();
            getnexttoken();
            return v;
        }
        std::cerr << "parser expects: integer or identifier\n";
        std::exit(1);
    }

    // scans every character of string in case there is a backslash or quote so that valid syntax is added
    static std::string escape(const std::string &s)
    {
        std::string out;
        out.reserve(s.size());
        for (char ch : s)
        {
            if (ch == '\\' || ch == '"')
                out.push_back('\\');
            out.push_back(ch);
        }
        return out;
    }

    void parse()
    {
        // begins with the header and main func that starts every file
        emitter.AddHeader("#include <stdio.h>");
        emitter.AddLine("int main(void) {");

        // runs statement
        while (!atend())
            statement();

        // concludes with every file ending which just returns 0
        emitter.AddLine("  return 0;");
        emitter.AddLine("}");
    }

    void statement()
    {
        // handles print branch
        if (checktype(Tokens::PRINT))
        {
            getnexttoken();
            // if there is a string, add proper syntax
            if (checktype(Tokens::STRING))
            {
                std::string text = escape(peektoken().value.value());
                emitter.AddLine("  printf(\"" + text + "\\n\");");
                getnexttoken();
            }
            // else add format speicficer
            else
            {
                std::string ex = expression();
                emitter.AddLine("  printf(\"%d\\n\", " + ex + ");");
            }
            semicolon();
        }
        // handles let branch
        else if (checktype(Tokens::LET))
        {
            getnexttoken();
            // expects an indentifier and stores it as a variable
            expect(Tokens::IDENT, "identifier after let");
            std::string var = last().value.value();
            // declares variable
            if (!emitter.symbols.count(var))
            {
                emitter.symbols[var] = "int";
                emitter.AddLine("  int " + var + ";");
            }
            expect(Tokens::ASSIGN, "=");
            std::string ex = expression();
            // assigns the variable
            emitter.AddLine("  " + var + " = " + ex + ";");
            semicolon();
        }
        // handles input branch
        else if (checktype(Tokens::INPUT))
        {
            getnexttoken();
            expect(Tokens::IDENT, "identifier after input");
            std::string var = last().value.value();
            if (emitter.symbols.find(var) == emitter.symbols.end())
            {
                emitter.symbols[var] = "int";
                emitter.AddLine("  int " + var + ";");
            }
            // scans for input from the user
            emitter.AddLine("  if (scanf(\"%d\", &" + var + ") != 1) " + var + " = 0;");
            semicolon();
        }
        // handles label branch
        else if (checktype(Tokens::LABEL))
        {
            getnexttoken();
            expect(Tokens::IDENT, "identifier after label");
            std::string lab = last().value.value();
            emitter.AddLine(lab + ": ;");
            semicolon();
        }
        // handles goto branch
        else if (checktype(Tokens::GOTO))
        {
            getnexttoken();
            expect(Tokens::IDENT, "identifier after goto");
            std::string lab = last().value.value();
            emitter.AddLine("  goto " + lab + ";");
            semicolon();
        }
        // constructs if statement with necessary parens
        else if (checktype(Tokens::IF))
        {
            getnexttoken();
            std::string cond = comparison();
            expect(Tokens::THEN, "then");
            emitter.AddLine("  if " + cond + " {");
            while (!checktype(Tokens::ENDIF) && !atend())
                statement();
            expect(Tokens::ENDIF, "endif");
            emitter.AddLine("  }");
        }
        // constructs while statement with necessary parens
        else if (checktype(Tokens::WHILE))
        {
            getnexttoken();
            std::string cond = comparison();
            expect(Tokens::REPEAT, "repeat");
            emitter.AddLine("  while " + cond + " {");
            while (!checktype(Tokens::ENDWHILE) && !atend())
                statement();
            expect(Tokens::ENDWHILE, "endwhile");
            emitter.AddLine("  }");
        }
        // else return error and exit
        else
        {
            std::cerr << "Unexpected token: " << tokenTypeToString(peektoken().type) << "\n";
            std::exit(1);
        }
    }
};

int main(int argc, char *argv[])
{
    // validate arg count
    if (argc != 2)
    {
        std::cerr << "incorrect usage\n";
        return 1;
    }

    // open file
    std::string words;
    {
        std::stringstream cont_stream;
        std::fstream input((argv[1]), std::ios::in);
        if (!input.is_open())
        {
            std::cerr << "Failed to open file: " << argv[1] << "\n";
            return 1;
        }
        cont_stream << input.rdbuf();
        words = cont_stream.str();
    }

    // tokenizer words (lexer) and then parse
    auto tokens = tokenizer(words);
    Emitter emitter;
    Parser parser(tokens, emitter);
    parser.parse();

    // write to a file
    fs::path inPath(argv[1]);
    fs::path outPath = inPath;
    outPath.replace_extension(".c");

    if (!outPath.parent_path().empty())
    {
        fs::create_directories(outPath.parent_path());
    }

    emitter.WriteToFile(outPath.string());
    std::cout << "WroteToFile: " << outPath << "\n";
    return 0;
}

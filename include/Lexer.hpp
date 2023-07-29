#pragma once
#include <string>
#include <vector>

class Lexer
{
public:
    enum class Lexeme
    {
        None,
        String,
        Word,
        Int,
        Float,
        BraceOpen,
        BraceClose,
        CurlyBraceOpen,
        CurlyBraceClose,
        Dot,
        Semicolon,
        Quote,
        Equal,
        Plus,
        Minus,
        Multiply,
        Divide
    };
    
    typedef std::pair<Lexer::Lexeme, std::string> Token;

    Lexer(const std::string& input);

    std::vector<Token> Result();

private:
    std::vector<Token> tokens;
};

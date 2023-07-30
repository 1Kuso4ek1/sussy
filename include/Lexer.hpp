#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "Utils.hpp"

class Lexer
{
public:
    enum class Lexeme
    {
        None,
        Word,
        ReservedWord,
        Int,
        Float,
        String,
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

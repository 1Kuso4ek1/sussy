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
        Bool,
        Float,
        String,
        BraceOpen,
        BraceClose,
        CurlyBraceOpen,
        CurlyBraceClose,
        Dot,
        Comma,
        Semicolon,
        Quote,
        Equal,
        Plus,
        Minus,
        Multiply,
        Divide,
        IsEqual,
        IsLess,
        IsGreater,
        IsLessOrEqual,
        IsGreaterOrEqual
    };
    
    using Token = std::pair<Lexer::Lexeme, std::string>;

    Lexer(const std::string& input);

    std::vector<Token> Result();

private:
    std::vector<Token> tokens;
};

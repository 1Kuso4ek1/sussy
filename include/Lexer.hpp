#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <filesystem>

#include "Utils.hpp"

class Lexer
{
public:
    enum class Lexeme
    {
        None,
        Word, ReservedWord,
        Int, Bool, Float, String,
        
        BraceOpen, BraceClose, CurlyBraceOpen, CurlyBraceClose, Arrow, Dot, Comma, Semicolon, Quote,
        
        Equal, Plus, Minus, Multiply, Divide, Pow, Mod, InRange, Colon,
        
        AddAssign, SubtractAssign, MultiplyAssign, DivideAssign,
        
        IsEqual, IsLess, IsGreater, IsLessOrEqual, IsGreaterOrEqual,

        BitwiseAnd, BitwiseOr, And, Or, LeftShift, RightShift
    };
    
    using Token = std::pair<Lexer::Lexeme, std::string>;

    Lexer(const std::string& inputFilename);

    std::vector<Token> Result();

private:
    void Tokenize(const std::string& input);
    void Import(const std::string& inputFilename);

    std::vector<Token> tokens;
    std::vector<Lexer> imported;
};

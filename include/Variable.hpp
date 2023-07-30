#pragma once
#include "Lexer.hpp"

class Variable
{
public:
    Variable& operator=(std::pair<Lexer::Lexeme, std::string> data);

    std::pair<Lexer::Lexeme, std::string> GetData();

private:
    std::pair<Lexer::Lexeme, std::string> data = { Lexer::Lexeme::None, "" };
};

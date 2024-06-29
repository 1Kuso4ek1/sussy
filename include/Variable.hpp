#pragma once
#include "Lexer.hpp"
#include <unordered_map>
#include <memory>

class Variable
{
public:
    Variable() {}
    Variable(Lexer::Token data) : data(data) {}

    Variable& operator=(Lexer::Token data);

    Lexer::Token GetData();

private:
    Lexer::Token data = { Lexer::Lexeme::None, "" };
};

using VarMap = std::unordered_map<std::string, std::shared_ptr<Variable>>;

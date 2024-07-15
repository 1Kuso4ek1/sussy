#pragma once
#include "Lexer.hpp"
#include <unordered_map>
#include <memory>

class Variable
{
public:
    enum class VariableType
    {
        Value,
        Array,
        Object
    };

    Variable() {}
    Variable(Lexer::Token data, VariableType type = VariableType::Value, int elements = 0) : data(data), type(type), array(elements) { }

    void SetType(VariableType type);
    void SetElement(int index, std::shared_ptr<Variable> element);
    void Fill(Lexer::Token data);

    Variable& operator=(Lexer::Token data);
    Variable& operator=(std::shared_ptr<Variable> variable);

    bool operator==(Lexer::Token data);
    bool operator==(std::shared_ptr<Variable> variable);

    std::shared_ptr<Variable> GetElement(int index);
    VariableType GetType();
    Lexer::Token GetData();

private:
    VariableType type = VariableType::Value;

    Lexer::Token data = { Lexer::Lexeme::None, "" };

    std::vector<std::shared_ptr<Variable>> array;
};

using VarMap = std::unordered_map<std::string, std::shared_ptr<Variable>>;

static VarMap variables;

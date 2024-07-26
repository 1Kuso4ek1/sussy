#pragma once
#include "Lexer.hpp"
#include <unordered_map>
#include <memory>
#include <any>

class Variable
{
public:
    enum class VariableType
    {
        Int,
        Float,
        Bool,
        String,
        Array,
        Object
    };

    Variable() : data(0) {}
    Variable(Lexer::Token type);
    Variable(std::shared_ptr<Variable> variable);
    Variable(std::any data, VariableType type = VariableType::Int, int elements = 0) : data(data), type(type), array(elements) { }

    void SetType(VariableType type);
    void SetData(std::any data);
    void SetElement(int index, std::shared_ptr<Variable> element);
    void Fill(std::any data);

    Variable& operator=(std::any data);
    Variable& operator=(std::shared_ptr<Variable> variable);

    bool operator==(std::any data);
    bool operator==(std::shared_ptr<Variable> variable);

    std::shared_ptr<Variable> GetElement(int index);
    VariableType GetType();
    std::any GetData();

private:
    VariableType type = VariableType::Int;

    std::any data;

    std::vector<std::shared_ptr<Variable>> array;
};

using VarMap = std::unordered_map<std::string, std::shared_ptr<Variable>>;

static VarMap variables;

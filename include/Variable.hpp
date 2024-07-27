#pragma once
#include "AST.hpp"

#include <unordered_map>
#include <memory>
#include <any>
#include <functional>
#include <iostream>

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
        Function,
        CppFunction,
        Object
    };

    Variable() : data(0) {}
    Variable(Lexer::Token type);
    Variable(std::shared_ptr<Variable> variable);
    Variable(std::any data, VariableType type = VariableType::Int, int elements = 0) : data(data), type(type), array(elements) { }
    Variable(AST::NodeList args, std::any body, VariableType type = VariableType::CppFunction) : data(body), args(args), type(type) { }

    void SetType(VariableType type);
    void SetData(std::any data);
    void SetElement(int index, std::shared_ptr<Variable> element);
    void Fill(std::any data);

    Variable& operator=(std::any data);
    Variable& operator=(std::shared_ptr<Variable> variable);

    bool operator==(std::any data);
    bool operator==(std::shared_ptr<Variable> variable);

    int GetArraySize();
    std::shared_ptr<Variable> GetElement(int index);
    VariableType GetType();
    std::any GetData();
    AST::NodeList GetArgs();

private:
    VariableType type = VariableType::Int;

    std::any data;

    AST::NodeList args;
    std::vector<std::shared_ptr<Variable>> array;
};

using VarMap = std::unordered_map<std::string, std::shared_ptr<Variable>>;

static std::shared_ptr<Variable> CallFuncForDiffTypes(Variable::VariableType type, std::vector<std::function<std::shared_ptr<Variable>(VarMap&)>> cases, VarMap& v)
{
    switch(type)
    {
    case Variable::VariableType::Int: return cases[0](v);
    case Variable::VariableType::Float: return cases[1](v);
    case Variable::VariableType::String: return cases[2](v);
    case Variable::VariableType::Bool: return cases[3](v);
    case Variable::VariableType::Array: return cases[4](v);
    default: return nullptr;
    }
}

static VarMap defaultVariables = 
{
    { "print", std::make_shared<Variable>(AST::NodeList{ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
        std::function<std::shared_ptr<Variable>(VarMap&)>(
        [](VarMap& v) -> std::shared_ptr<Variable>
        {
            return CallFuncForDiffTypes(v["value"]->GetType(),
            {
                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<int>(v["value"]->GetData()); return nullptr; },
                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<float>(v["value"]->GetData()); return nullptr; },
                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<std::string>(v["value"]->GetData()); return nullptr; },
                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << (std::any_cast<bool>(v["value"]->GetData()) ? "true" : "false"); return nullptr; }
            }, v);
        })) },
    { "println", std::make_shared<Variable>(AST::NodeList{ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        std::function<std::shared_ptr<Variable>(VarMap&)>(
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<int>(v["value"]->GetData()) << std::endl; return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<float>(v["value"]->GetData()) << std::endl; return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<std::string>(v["value"]->GetData()) << std::endl; return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << (std::any_cast<bool>(v["value"]->GetData()) ? "true" : "false") << std::endl; return nullptr; }
                            }, v);
                        }))},

    { "input", std::make_shared<Variable>(AST::NodeList{ }, std::function<std::shared_ptr<Variable>(VarMap&)>([](VarMap& v) -> std::shared_ptr<Variable> { std::string ret; std::getline(std::cin, ret); return std::make_shared<Variable>(Lexer::Token(Lexer::Lexeme::String, ret)); })) },
    { "int", std::make_shared<Variable>(AST::NodeList{ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        std::function<std::shared_ptr<Variable>(VarMap&)>(
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { return v["value"]; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((int)std::any_cast<float>(v["value"]->GetData()), Variable::VariableType::Int); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::stoi(std::any_cast<std::string>(v["value"]->GetData())), Variable::VariableType::Int); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((int)std::any_cast<bool>(v["value"]->GetData()), Variable::VariableType::Int); }
                            }, v);
                        })) },

    { "float", std::make_shared<Variable>(AST::NodeList{ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        std::function<std::shared_ptr<Variable>(VarMap&)>(
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((float)std::any_cast<int>(v["value"]->GetData()), Variable::VariableType::Float); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return v["value"]; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::stof(std::any_cast<std::string>(v["value"]->GetData())), Variable::VariableType::Float); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((float)std::any_cast<bool>(v["value"]->GetData()), Variable::VariableType::Float); }
                            }, v);
                        })) },

    { "string", std::make_shared<Variable>(AST::NodeList{ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        std::function<std::shared_ptr<Variable>(VarMap&)>(
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::to_string(std::any_cast<int>(v["value"]->GetData())), Variable::VariableType::String); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::to_string(std::any_cast<float>(v["value"]->GetData())), Variable::VariableType::String); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return v["value"]; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::string(std::any_cast<bool>(v["value"]->GetData()) ? "true" : "false"), Variable::VariableType::String); }
                            }, v);
                        })) },

    { "size", std::make_shared<Variable>(AST::NodeList{ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        std::function<std::shared_ptr<Variable>(VarMap&)>(
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((int)sizeof(std::any_cast<int>(v["value"]->GetData())), Variable::VariableType::Int); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((int)sizeof(std::any_cast<float>(v["value"]->GetData())), Variable::VariableType::Int); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((int)std::any_cast<std::string>(v["value"]->GetData()).size(), Variable::VariableType::Int); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((int)sizeof(std::any_cast<bool>(v["value"]->GetData()) ? "true" : "false"), Variable::VariableType::Int); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(v["value"]->GetArraySize(), Variable::VariableType::Int); }
                            }, v);
                        })) }
};

#pragma once
#include <functional>
#include <iostream>

#include "AST.hpp"
#include "Variable.hpp"

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

class Function
{
public:
    Function() {}
    Function(AST::NodeList args, std::shared_ptr<AST::Node> body)
            : args(args), body(body) {}
    Function(AST::NodeList args, std::function<std::shared_ptr<Variable>(VarMap&)> cppbody)
            : args(args), cppbody(cppbody) {}

    void SetArgs(std::vector<std::shared_ptr<Variable>> args, VarMap& vars);

    int GetArgsCount();

    VarMap& GetLocalVariables();
    std::shared_ptr<AST::Node> GetBody();
    std::shared_ptr<Variable> Execute();

private:
    VarMap localVariables;

    std::vector<std::shared_ptr<AST::Node>> args;
    std::shared_ptr<AST::Node> body;
    std::function<std::shared_ptr<Variable>(VarMap&)> cppbody;
};

using FunctionMap = std::unordered_map<std::string, Function>;

static FunctionMap functions = 
{
    { "print", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<int>(v["value"]->GetData()); return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<float>(v["value"]->GetData()); return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<std::string>(v["value"]->GetData()); return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << (std::any_cast<bool>(v["value"]->GetData()) ? "true" : "false"); return nullptr; }
                            }, v);
                        })},

    { "println", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<int>(v["value"]->GetData()) << std::endl; return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<float>(v["value"]->GetData()) << std::endl; return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << std::any_cast<std::string>(v["value"]->GetData()) << std::endl; return nullptr; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { std::cout << (std::any_cast<bool>(v["value"]->GetData()) ? "true" : "false") << std::endl; return nullptr; }
                            }, v);
                        })},

    { "input", Function({ }, [](VarMap& v) -> std::shared_ptr<Variable> { std::string ret; std::cin >> ret; return std::make_shared<Variable>(Lexer::Token(Lexer::Lexeme::String, ret)); }) },
    { "int", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { return v["value"]; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((int)std::any_cast<float>(v["value"]->GetData()), Variable::VariableType::Int); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::stoi(std::any_cast<std::string>(v["value"]->GetData())), Variable::VariableType::Int); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((int)std::any_cast<bool>(v["value"]->GetData()), Variable::VariableType::Int); }
                            }, v);
                        }) },

    { "float", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((float)std::any_cast<int>(v["value"]->GetData()), Variable::VariableType::Float); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return v["value"]; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::stof(std::any_cast<std::string>(v["value"]->GetData())), Variable::VariableType::Float); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>((float)std::any_cast<bool>(v["value"]->GetData()), Variable::VariableType::Float); }
                            }, v);
                        }) },

    { "string", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        [](VarMap& v) -> std::shared_ptr<Variable>
                        {
                            return CallFuncForDiffTypes(v["value"]->GetType(),
                            {
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::to_string(std::any_cast<int>(v["value"]->GetData())), Variable::VariableType::String); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::to_string(std::any_cast<float>(v["value"]->GetData())), Variable::VariableType::String); },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return v["value"]; },
                                [](VarMap& v) -> std::shared_ptr<Variable> { return std::make_shared<Variable>(std::string(std::any_cast<bool>(v["value"]->GetData()) ? "true" : "false"), Variable::VariableType::String); }
                            }, v);
                        }) },

    { "size", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
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
                        }) }
};

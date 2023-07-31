#pragma once
#include "AST.hpp"
#include "Variable.hpp"

class Function
{
public:
    Function() {}
    Function(std::vector<std::shared_ptr<AST::Node>> args, std::shared_ptr<AST::Node> body)
            : args(args), body(body) {}

    void SetArgs(std::vector<Lexer::Token> args, std::unordered_map<std::string, std::shared_ptr<Variable>>& vars);

    std::unordered_map<std::string, std::shared_ptr<Variable>>& GetLocalVariables();
    std::shared_ptr<AST::Node> GetBody();

private:
    std::unordered_map<std::string, std::shared_ptr<Variable>> localVariables;

    std::vector<std::shared_ptr<AST::Node>> args;
    std::shared_ptr<AST::Node> body;
};

static std::unordered_map<std::string, Function> functions;

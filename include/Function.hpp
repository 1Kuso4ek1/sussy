#pragma once
#include "AST.hpp"
#include "Variable.hpp"
#include <functional>

class Function
{
public:
    Function() {}
    Function(std::vector<std::shared_ptr<AST::Node>> args, std::shared_ptr<AST::Node> body)
            : args(args), body(body) {}
    Function(std::vector<std::shared_ptr<AST::Node>> args, std::function<Lexer::Token(VarMap)> cppbody)
            : args(args), cppbody(cppbody) {}

    void SetArgs(std::vector<Lexer::Token> args, VarMap& vars);

    VarMap& GetLocalVariables();
    std::shared_ptr<AST::Node> GetBody();
    Lexer::Token Execute();

private:
    VarMap localVariables;

    std::vector<std::shared_ptr<AST::Node>> args;
    std::shared_ptr<AST::Node> body;
    std::function<Lexer::Token(VarMap)> cppbody;
};

static std::unordered_map<std::string, Function> functions;

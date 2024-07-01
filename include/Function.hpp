#pragma once
#include "AST.hpp"
#include "Variable.hpp"
#include <functional>
#include <iostream>

class Function
{
public:
    Function() {}
    Function(AST::NodeList args, std::shared_ptr<AST::Node> body)
            : args(args), body(body) {}
    Function(AST::NodeList args, std::function<Lexer::Token(VarMap)> cppbody)
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

static std::unordered_map<std::string, Function> functions = 
{
    { "print", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                        [](VarMap v) -> Lexer::Token { std::cout << v["value"]->GetData().second; return { Lexer::Lexeme::None, "" }; }) },
    { "println", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                          [](VarMap v) -> Lexer::Token { std::cout << v["value"]->GetData().second << std::endl; return { Lexer::Lexeme::None, "" }; }) },
    { "input", Function({ }, [](VarMap v) -> Lexer::Token { std::string ret; std::cin >> ret; return { Lexer::Lexeme::String, ret }; }) },
    { "int", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                          [](VarMap v) -> Lexer::Token { return { Lexer::Lexeme::Int, v["value"]->GetData().second }; }) },
    { "float", Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                          [](VarMap v) -> Lexer::Token { return { Lexer::Lexeme::Float, v["value"]->GetData().second }; }) }
};

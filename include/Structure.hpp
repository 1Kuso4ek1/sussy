#pragma once
#include "AST.hpp"
#include "Variable.hpp"

class Struct
{
public:
    Struct() {}
    Struct(std::shared_ptr<AST::Node> body) : body(body) {};

    std::shared_ptr<AST::Node> GetBody();

private:
    std::shared_ptr<AST::Node> body;

    Variable::VarMap localVariables;
};

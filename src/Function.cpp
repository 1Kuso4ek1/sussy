#include <Function.hpp>
#include <Exec.hpp>
#include <iostream>

void Function::SetArgs(std::vector<Lexer::Token> args, VarMap& vars)
{
    localVariables.clear();

    for(int i = 0; i < this->args.size(); i++)
    {
        if(i < args.size())
            localVariables[this->args[i]->expression.second] = std::make_shared<Variable>(args[i]);
        else if(this->args[i]->expression.first != Lexer::Lexeme::Word)
            localVariables[this->args[i]->children[0]->expression.second] = std::make_shared<Variable>(GetReturn(this->args[i], vars));
    }
    for(auto [name, var] : vars)
        localVariables[name] = var;
}

VarMap& Function::GetLocalVariables()
{
    return localVariables;
}

std::shared_ptr<AST::Node> Function::GetBody()
{
    return body;
}

Lexer::Token Function::Execute()
{
    return cppbody(localVariables);
}

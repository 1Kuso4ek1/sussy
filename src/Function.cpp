#include <Function.hpp>

void Function::SetArgs(std::vector<Lexer::Token> args, VarMap& vars)
{
    localVariables.clear();

    for(int i = 0; i < this->args.size(); i++)
        localVariables[this->args[i]->expression.second] = std::make_shared<Variable>(args[i]);
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

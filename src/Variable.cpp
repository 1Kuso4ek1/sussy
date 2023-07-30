#include <Variable.hpp>

Variable& Variable::operator=(std::pair<Lexer::Lexeme, std::string> data)
{
    this->data = data;
    return *this;
}

std::pair<Lexer::Lexeme, std::string> Variable::GetData()
{
    return data;
}

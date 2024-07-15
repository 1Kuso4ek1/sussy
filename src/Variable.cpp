#include <Variable.hpp>

void Variable::SetType(VariableType type)
{
    this->type = type;
}

void Variable::SetElement(int index, std::shared_ptr<Variable> element)
{
    if(index < array.size())
        array[index] = element;
    else
        array.push_back(element);
}

void Variable::Fill(Lexer::Token data)
{
    if(type == VariableType::Array)
        for(auto& i : array)
            i = std::make_shared<Variable>(data);
}

Variable& Variable::operator=(Lexer::Token data)
{
    this->data = data;
    array.clear();
    type = VariableType::Value;

    return *this;
}

Variable& Variable::operator=(std::shared_ptr<Variable> variable)
{
    data = variable->data;
    type = variable->type;
    array = variable->array;

    return *this;
}

bool Variable::operator==(Lexer::Token data)
{
    return this->data == data;
}

bool Variable::operator==(std::shared_ptr<Variable> variable)
{
    return this->data == variable->data;
}

std::shared_ptr<Variable> Variable::GetElement(int index)
{
    if(index < array.size())
        return array[index];

    return nullptr;
}

Variable::VariableType Variable::GetType()
{
    return type;
}

Lexer::Token Variable::GetData()
{
    return data;
}

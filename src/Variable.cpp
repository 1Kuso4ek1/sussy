#include <Variable.hpp>
#include <iostream>
Variable::Variable(Lexer::Token type)
{
    switch(type.first)
    {
    case Lexer::Lexeme::Word:
    case Lexer::Lexeme::String:
        this->type = VariableType::String;
        this->data = type.second;
        break;
    case Lexer::Lexeme::Int:
        this->type = VariableType::Int;
        this->data = std::stoi(type.second);
        break;
    case Lexer::Lexeme::Float:
        this->type = VariableType::Float;
        this->data = std::stof(type.second);
        break;
    case Lexer::Lexeme::Bool:
        this->type = VariableType::Bool;
        this->data = (type.second == "true" ? true : false);
        break;
    default:
        this->type = VariableType::Int;
        this->data = 0;
        break;
    }
}

Variable::Variable(std::shared_ptr<Variable> variable)
{
    this->type = variable->type;
    this->data = variable->data;
    this->array = variable->array;
}

void Variable::SetType(VariableType type)
{
    this->type = type;
}

void Variable::SetData(std::any data)
{
    this->data = data;
}

void Variable::SetElement(int index, std::shared_ptr<Variable> element)
{
    if(index < array.size())
        array[index] = element;
    else
        array.push_back(element);
}

void Variable::Fill(std::any data)
{
    if(type == VariableType::Array)
        for(auto& i : array)
            i = std::make_shared<Variable>(data);
}

Variable& Variable::operator=(std::any data)
{
    this->data = data;
    array.clear();
    type = VariableType::Int;

    return *this;
}

Variable& Variable::operator=(std::shared_ptr<Variable> variable)
{
    data = variable->data;
    type = variable->type;
    array = variable->array;

    return *this;
}

bool Variable::operator==(std::any data)
{
    return false;//this->data == data;
}

bool Variable::operator==(std::shared_ptr<Variable> variable)
{
    switch(type)
    {
    case VariableType::Int:
        return std::any_cast<int>(data) == std::any_cast<int>(variable->data);
    case VariableType::Float:
        return std::any_cast<float>(data) == std::any_cast<float>(variable->data);
    case VariableType::String:
        return std::any_cast<std::string>(data) == std::any_cast<std::string>(variable->data);
    case VariableType::Array:
        if(array.size() != variable->array.size())
            return false;
        for(int i = 0; i < array.size(); i++)
            if(!(*array[i] == *variable->array[i]))
                return false;
        return true;
    }

    return false;
}

int Variable::GetArraySize()
{
    return array.size();
}

std::shared_ptr<Variable> Variable::GetElement(int index)
{
    if(index < array.size())
        return array[index];

    std::cout << index << std::endl;
    return nullptr;
}

Variable::VariableType Variable::GetType()
{
    return type;
}

std::any Variable::GetData()
{
    return data;
}

AST::NodeList Variable::GetArgs()
{
    return args;
}

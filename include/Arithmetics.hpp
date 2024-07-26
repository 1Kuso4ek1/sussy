#pragma once
#include <cmath>
#include <iostream>

#include "Lexer.hpp"
#include "Variable.hpp"

static std::shared_ptr<Variable> Index(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    if(left->GetType() == Variable::VariableType::String)
        return std::make_shared<Variable>(std::string(1, std::any_cast<std::string>(left->GetData())[std::any_cast<int>(right->GetData())]), Variable::VariableType::String);

    return right;
}

static std::shared_ptr<Variable> IsLess(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) < std::any_cast<int>(right->GetData()), Variable::VariableType::Bool);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::any_cast<float>(left->GetData()) < std::any_cast<float>(right->GetData()), Variable::VariableType::Bool);
    case Variable::VariableType::String:
        return std::make_shared<Variable>(std::any_cast<std::string>(left->GetData()).size() < std::any_cast<std::string>(right->GetData()).size(), Variable::VariableType::Bool);
    }

    return std::make_shared<Variable>(false, Variable::VariableType::Bool);
}

static std::shared_ptr<Variable> IsGreater(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) > std::any_cast<int>(right->GetData()), Variable::VariableType::Bool);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::any_cast<float>(left->GetData()) > std::any_cast<float>(right->GetData()), Variable::VariableType::Bool);
    case Variable::VariableType::String:
        return std::make_shared<Variable>(std::any_cast<std::string>(left->GetData()).size() > std::any_cast<std::string>(right->GetData()).size(), Variable::VariableType::Bool);
    }

    return std::make_shared<Variable>(false, Variable::VariableType::Bool);
}

static std::shared_ptr<Variable> IsLessOrEqual(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) <= std::any_cast<int>(right->GetData()), Variable::VariableType::Bool);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::any_cast<float>(left->GetData()) <= std::any_cast<float>(right->GetData()), Variable::VariableType::Bool);
    case Variable::VariableType::String:
        return std::make_shared<Variable>(std::any_cast<std::string>(left->GetData()).size() <= std::any_cast<std::string>(right->GetData()).size(), Variable::VariableType::Bool);
    }

    return std::make_shared<Variable>(false, Variable::VariableType::Bool);
}

static std::shared_ptr<Variable> IsGreaterOrEqual(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) >= std::any_cast<int>(right->GetData()), Variable::VariableType::Bool);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::any_cast<float>(left->GetData()) >= std::any_cast<float>(right->GetData()), Variable::VariableType::Bool);
    case Variable::VariableType::String:
        return std::make_shared<Variable>(std::any_cast<std::string>(left->GetData()).size() >= std::any_cast<std::string>(right->GetData()).size(), Variable::VariableType::Bool);
    }

    return std::make_shared<Variable>(false, Variable::VariableType::Bool);
}

static std::shared_ptr<Variable> IsEqual(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    return std::make_shared<Variable>(*left == right, Variable::VariableType::Bool);
}

static std::shared_ptr<Variable> Not(std::shared_ptr<Variable> value)
{
    return std::make_shared<Variable>(!std::any_cast<bool>(value->GetData()), Variable::VariableType::Bool);
}

static std::shared_ptr<Variable> And(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    return std::make_shared<Variable>(std::any_cast<bool>(left->GetData()) && std::any_cast<bool>(right->GetData()), Variable::VariableType::Bool);
}

static std::shared_ptr<Variable> Or(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    return std::make_shared<Variable>(std::any_cast<bool>(left->GetData()) || std::any_cast<bool>(right->GetData()), Variable::VariableType::Bool);
}

static std::shared_ptr<Variable> BitwiseAnd(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) & std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
}

static std::shared_ptr<Variable> BitwiseOr(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) | std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
}

static std::shared_ptr<Variable> LeftShift(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) << std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
}

static std::shared_ptr<Variable> RightShift(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) >> std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
}

static std::shared_ptr<Variable> Decrement(std::shared_ptr<Variable> value)
{
    return std::make_shared<Variable>(std::any_cast<int>(value->GetData()) - 1, Variable::VariableType::Int);
}

static std::shared_ptr<Variable> Increment(std::shared_ptr<Variable> value)
{
    return std::make_shared<Variable>(std::any_cast<int>(value->GetData()) + 1, Variable::VariableType::Int);
}

static std::shared_ptr<Variable> Add(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) + std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::any_cast<float>(left->GetData()) + std::any_cast<float>(right->GetData()), Variable::VariableType::Float);
    case Variable::VariableType::String:
        return std::make_shared<Variable>(std::any_cast<std::string>(left->GetData()) + std::any_cast<std::string>(right->GetData()), Variable::VariableType::String);
    }

    return std::make_shared<Variable>();
}

static std::shared_ptr<Variable> Subtract(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) - std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::any_cast<float>(left->GetData()) - std::any_cast<float>(right->GetData()), Variable::VariableType::Float);
    }

    return std::make_shared<Variable>();
}

static std::shared_ptr<Variable> Negate(std::shared_ptr<Variable> number)
{
    switch(number->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(-std::any_cast<int>(number->GetData()), Variable::VariableType::Int);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(-std::any_cast<float>(number->GetData()), Variable::VariableType::Float);
    }

    return std::make_shared<Variable>();
}

static std::shared_ptr<Variable> Multiply(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) * std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::any_cast<float>(left->GetData()) * std::any_cast<float>(right->GetData()), Variable::VariableType::Float);
    }

    return std::make_shared<Variable>();
}

static std::shared_ptr<Variable> Divide(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) / std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::any_cast<float>(left->GetData()) / std::any_cast<float>(right->GetData()), Variable::VariableType::Float);
    }

    return std::make_shared<Variable>();
}

static std::shared_ptr<Variable> Pow(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(int(std::pow(std::any_cast<int>(left->GetData()), std::any_cast<int>(right->GetData()))), Variable::VariableType::Int);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(std::pow(std::any_cast<float>(left->GetData()), std::any_cast<float>(right->GetData())), Variable::VariableType::Float);
    }

    return std::make_shared<Variable>();
}

static std::shared_ptr<Variable> Mod(std::shared_ptr<Variable> left, std::shared_ptr<Variable> right)
{
    switch(left->GetType())
    {
    case Variable::VariableType::Int:
        return std::make_shared<Variable>(std::any_cast<int>(left->GetData()) % std::any_cast<int>(right->GetData()), Variable::VariableType::Int);
    case Variable::VariableType::Float:
        return std::make_shared<Variable>(fmod(std::any_cast<float>(left->GetData()), std::any_cast<float>(right->GetData())), Variable::VariableType::Float);
    }

    return std::make_shared<Variable>();
}

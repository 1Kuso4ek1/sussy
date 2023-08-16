#pragma once
#include "Lexer.hpp"

static Lexer::Token IsEqual(Lexer::Token left, Lexer::Token right)
{
    return { Lexer::Lexeme::Bool, left.second == right.second ? "true" : "false" };
}

static Lexer::Token Add(Lexer::Token left, Lexer::Token right)
{
    switch (left.first)
    {
    case Lexer::Lexeme::Int:
        return { Lexer::Lexeme::Int, std::to_string(stoi(left.second) + stoi(right.second)) };
    case Lexer::Lexeme::Float:
        return { Lexer::Lexeme::Float, std::to_string(stof(left.second) + stof(right.second)) };
    case Lexer::Lexeme::String:
        return { Lexer::Lexeme::String, left.second + right.second };
    }

    return { Lexer::Lexeme::None, "" };
}

static Lexer::Token Subtract(Lexer::Token left, Lexer::Token right)
{
    switch (left.first)
    {
    case Lexer::Lexeme::Int:
        return { Lexer::Lexeme::Int, std::to_string(stoi(left.second) - stoi(right.second)) };
    case Lexer::Lexeme::Float:
        return { Lexer::Lexeme::Float, std::to_string(stof(left.second) - stof(right.second)) };
    }

    return { Lexer::Lexeme::None, "" };
}

static Lexer::Token Multiply(Lexer::Token left, Lexer::Token right)
{
    switch (left.first)
    {
    case Lexer::Lexeme::Int:
        return { Lexer::Lexeme::Int, std::to_string(stoi(left.second) * stoi(right.second)) };
    case Lexer::Lexeme::Float:
        return { Lexer::Lexeme::Float, std::to_string(stof(left.second) * stof(right.second)) };
    }

    return { Lexer::Lexeme::None, "" };
}

static Lexer::Token Divide(Lexer::Token left, Lexer::Token right)
{
    switch (left.first)
    {
    case Lexer::Lexeme::Int:
        return { Lexer::Lexeme::Int, std::to_string(stoi(left.second) / stoi(right.second)) };
    case Lexer::Lexeme::Float:
        return { Lexer::Lexeme::Float, std::to_string(stof(left.second) / stof(right.second)) };
    }

    return { Lexer::Lexeme::None, "" };
}

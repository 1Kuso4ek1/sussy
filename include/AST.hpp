#pragma once
#include "Lexer.hpp"
#include "Arithmetics.hpp"
#include <memory>

class AST
{
public:
    struct Node
    {
        Node(Lexer::Token expression) : expression(expression) {}

        Lexer::Token GetReturn()
        {
            if(children.empty())
                return expression;

            auto leftRet = children[0]->GetReturn();
            auto rightRet = children.back()->GetReturn();

            switch(expression.first)
            {
            case Lexer::Lexeme::Plus: return Add(leftRet, rightRet); break;
            case Lexer::Lexeme::Minus: return Subtract(leftRet, rightRet); break;
            case Lexer::Lexeme::Multiply: return Multiply(leftRet, rightRet); break;
            case Lexer::Lexeme::Divide: return Divide(leftRet, rightRet); break;
            }

            return { Lexer::Lexeme::None, "" };
        }

        Lexer::Token expression;

        std::vector<std::shared_ptr<Node>> children;
    };

    AST(const std::vector<Lexer::Token>& tokens);

    std::shared_ptr<Node> GetRootNode();

private:
    std::shared_ptr<Node> root;

    //std::vector<std::shared_ptr<Node>> nodes;
};

#pragma once
#include "Lexer.hpp"
#include "Arithmetics.hpp"
#include <memory>
#include <stack>

class AST
{
public:
    struct Node
    {
        Node(Lexer::Token expression) : expression(expression) {}

        Lexer::Token expression;

        std::vector<std::shared_ptr<Node>> children;
    };

    AST(std::vector<Lexer::Token>& tokens);

    std::shared_ptr<Node> GetRootNode();

private:
    int GetOperatorPriority(Lexer::Lexeme lexeme);

    std::shared_ptr<Node> root;
};

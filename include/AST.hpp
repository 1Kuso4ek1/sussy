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

        Lexer::Token expression;

        std::vector<std::shared_ptr<Node>> children;
    };

    AST(const std::vector<Lexer::Token>& tokens);

    std::shared_ptr<Node> GetRootNode();

private:
    std::shared_ptr<Node> root;
};

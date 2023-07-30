#include <AST.hpp>

AST::AST(const std::vector<Lexer::Token>& tokens)
{
    root = std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, ""));
    root->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
    std::shared_ptr<Node> currentNode = root->children[0];

    for(auto i = tokens.begin(); i < tokens.end(); i++)
    {
        switch (i->first)
        {
        case Lexer::Lexeme::Word:
        case Lexer::Lexeme::Int:
        case Lexer::Lexeme::Float:
        case Lexer::Lexeme::String:
            currentNode->expression = *i;
            break;

        case Lexer::Lexeme::Equal:
        case Lexer::Lexeme::Plus:
        case Lexer::Lexeme::Minus:
        case Lexer::Lexeme::Multiply:
        case Lexer::Lexeme::Divide:
            currentNode->expression = *i;
            currentNode->children.emplace_back(std::make_shared<Node>(*(i - 1)));
            currentNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = currentNode->children.back();
            break;

        case Lexer::Lexeme::Semicolon:
            root->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = root->children.back();
            break;
        
        default:
            break;
        }
    }
}

std::shared_ptr<AST::Node> AST::GetRootNode()
{
    return root;
}

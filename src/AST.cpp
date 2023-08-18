#include <AST.hpp>

AST::AST(const std::vector<Lexer::Token>& tokens)
{
    root = std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, ""));
    root->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
    std::shared_ptr<Node> currentNode = root->children[0];
    std::shared_ptr<Node> prevNode = root->children[0];   // make it
    std::shared_ptr<Node> prevNode1 = root->children[0];  // BETTER
    std::vector<std::shared_ptr<Node>> curlyBraceNodes = { root };

    bool braceOpen = false;
    bool curlyBraceOpen = false;

    for(auto i = tokens.begin(); i < tokens.end(); i++)
    {
        switch (i->first)
        {
        case Lexer::Lexeme::ReservedWord:
        case Lexer::Lexeme::Word:
        case Lexer::Lexeme::Int:
        case Lexer::Lexeme::Float:
        case Lexer::Lexeme::String:
            currentNode->expression = *i;
            break;

        case Lexer::Lexeme::CurlyBraceOpen: // not as good as might be!
            curlyBraceOpen = true;
            currentNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = currentNode->children.back();
            curlyBraceNodes.push_back(currentNode);
        case Lexer::Lexeme::BraceOpen:      // not as good as might be!
            if(i->first == Lexer::Lexeme::BraceOpen) braceOpen = true;
            if(currentNode->expression.first != Lexer::Lexeme::None)
            {
                currentNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
                currentNode = currentNode->children.back();
            }

            currentNode->expression = *i;
            currentNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            prevNode1 = prevNode;
            prevNode = currentNode;
            currentNode = currentNode->children.back();
            break;

        case Lexer::Lexeme::CurlyBraceClose:
            curlyBraceOpen = false;
            curlyBraceNodes.pop_back();
            curlyBraceNodes.back()->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = curlyBraceNodes.back()->children.back();
            break;

        case Lexer::Lexeme::BraceClose:
            if(i->first == Lexer::Lexeme::BraceClose)
            {
                braceOpen = false;
                currentNode = prevNode;
            }
            break;
        
        case Lexer::Lexeme::Comma:
            prevNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = prevNode->children.back();
            break;

        case Lexer::Lexeme::IsLess:
        case Lexer::Lexeme::IsGreater:
        case Lexer::Lexeme::IsLessOrEqual:
        case Lexer::Lexeme::IsGreaterOrEqual:
        case Lexer::Lexeme::IsEqual:
        case Lexer::Lexeme::Equal:
        case Lexer::Lexeme::Plus:
        case Lexer::Lexeme::Minus:
        case Lexer::Lexeme::Multiply:
        case Lexer::Lexeme::Divide:
            currentNode->expression = *i;
            currentNode->children.emplace_back(std::make_shared<Node>(*(i - 1)));
            currentNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            if(!braceOpen)
            {
                prevNode1 = prevNode;
                prevNode = currentNode;
            }
            currentNode = currentNode->children.back();
            break;

        case Lexer::Lexeme::Semicolon:
            curlyBraceNodes.back()->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = curlyBraceNodes.back()->children.back();
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

#include <AST.hpp>

AST::AST(const std::vector<Lexer::Token>& tokens)
{
    root = std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, ""));
    root->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
    std::shared_ptr<Node> currentNode = root->children[0];
    std::shared_ptr<Node> prevNode = root->children[0];   // make it
    std::shared_ptr<Node> prevNode1 = root->children[0];  // BETTER
    std::shared_ptr<Node> curlyBraceNode = nullptr;       // PLEASE

    bool braceOpen = false;
    bool curlyBraceOpen = false;

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

        case Lexer::Lexeme::CurlyBraceOpen: // not as good as might be!
            curlyBraceOpen = true;
            currentNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = currentNode->children.back();
            curlyBraceNode = currentNode;
        case Lexer::Lexeme::BraceOpen:      // not as good as might be!
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
            root->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = root->children.back();
            break;

        case Lexer::Lexeme::BraceClose:
            currentNode = prevNode1;
            break;
        
        case Lexer::Lexeme::Comma:
            prevNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            currentNode = prevNode->children.back();
            break;

        case Lexer::Lexeme::Equal:
        case Lexer::Lexeme::Plus:
        case Lexer::Lexeme::Minus:
        case Lexer::Lexeme::Multiply:
        case Lexer::Lexeme::Divide:
            currentNode->expression = *i;
            currentNode->children.emplace_back(std::make_shared<Node>(*(i - 1)));
            currentNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            prevNode1 = prevNode;
            prevNode = currentNode;
            currentNode = currentNode->children.back();
            break;

        case Lexer::Lexeme::Semicolon:
            if(curlyBraceOpen)
            {
                curlyBraceNode->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
                currentNode = curlyBraceNode->children.back();
                break;
            }

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

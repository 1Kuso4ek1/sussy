#include <AST.hpp>

AST::AST(std::vector<Lexer::Token>& tokens)
{
    /*
    root = std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, ""));
    root->children.emplace_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
    std::shared_ptr<Node> currentNode = root->children[0];
    std::shared_ptr<Node> prevNode = root->children[0];   // make it
    std::shared_ptr<Node> prevNode1 = root->children[0];  // BETTER
    std::vector<std::shared_ptr<Node>> curlyBraceNodes = { root };

    bool braceOpen = false;
    bool curlyBraceOpen = false;
    bool negative = false;

    for(auto i = tokens.begin(); i < tokens.end(); i++)
    {
        switch (i->first)
        {
        case Lexer::Lexeme::ReservedWord:
        case Lexer::Lexeme::Word:
            if(negative)
            {
                prevNode->expression.first = Lexer::Lexeme::Minus;
                prevNode->expression.second = "-";
                negative = false;
            }
        case Lexer::Lexeme::Int:
        case Lexer::Lexeme::Float:
            if(negative)
            {
                i->second.insert(0, "-");
                negative = false;
            }
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
            if(i->first == Lexer::Lexeme::Minus)
            {
                i->first = Lexer::Lexeme::Plus;
                i->second = "+";
                negative = true;
            }
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
    */
    
    std::stack<std::shared_ptr<Node>> values, operators;

    auto addUnaryChild = [&]()
    {
        auto operand = values.top(); values.pop();
        auto node = operators.top(); operators.pop();

        node->children.push_back(operand);

        values.push(node);
    };
    
    auto addChild = [&]()
    {
        auto right = values.top(); values.pop();
        auto left = values.top(); values.pop();
        auto node = operators.top(); operators.pop();

        node->children.push_back(left);
        node->children.push_back(right);

        values.push(node);
    };

    for(auto i = tokens.begin(); i < tokens.end(); i++)
    {
        switch(i->first)
        {
        case Lexer::Lexeme::Int:
        case Lexer::Lexeme::Float:
            values.push(std::make_shared<Node>(*i));
            break;

        case Lexer::Lexeme::BraceOpen:
            operators.push(std::make_shared<Node>(*i));
            break;
        case Lexer::Lexeme::BraceClose:
            while(!operators.empty() && operators.top()->expression.first != Lexer::Lexeme::BraceOpen)
                addChild();
            operators.pop();
            break;

        case Lexer::Lexeme::Minus:
        case Lexer::Lexeme::Plus:
        case Lexer::Lexeme::Comma:
        case Lexer::Lexeme::IsLess:
        case Lexer::Lexeme::IsGreater:
        case Lexer::Lexeme::IsLessOrEqual:
        case Lexer::Lexeme::IsGreaterOrEqual:
        case Lexer::Lexeme::IsEqual:
        case Lexer::Lexeme::Equal:
        case Lexer::Lexeme::Multiply:
        case Lexer::Lexeme::Divide:
            while(!operators.empty() && GetOperatorPriority(operators.top()->expression.first) >= GetOperatorPriority(i->first))
                addChild();
            operators.push(std::make_shared<Node>(*i));
            break;

        default:
            break;
        }
    }

    while(!operators.empty())
        addChild();

    root = values.top();
}

std::shared_ptr<AST::Node> AST::GetRootNode()
{
    return root;
}

int AST::GetOperatorPriority(Lexer::Lexeme lexeme)
{
    switch(lexeme)
    {
    case Lexer::Lexeme::Comma: return 17;
    case Lexer::Lexeme::IsLess: return 2;
    case Lexer::Lexeme::IsGreater: return 2;
    case Lexer::Lexeme::IsLessOrEqual: return 2;
    case Lexer::Lexeme::IsGreaterOrEqual: return 2;
    case Lexer::Lexeme::IsEqual: return 1;
    case Lexer::Lexeme::Equal: return 1;
    case Lexer::Lexeme::Plus: return 3;
    case Lexer::Lexeme::Minus: return 3;
    case Lexer::Lexeme::Multiply: return 4;
    case Lexer::Lexeme::Divide: return 4;
    
    default: return 0;
    }
}

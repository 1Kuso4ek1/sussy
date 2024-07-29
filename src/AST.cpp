#include <AST.hpp>

AST::AST(std::vector<Lexer::Token>& tokens)
{
    root = std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "root"));

    std::stack<std::shared_ptr<Node>> values, operators, curlyBraces;
    //std::shared_ptr<Node> bottomOperator;

    auto addUnaryChild = [&]()
    {
        auto operand = values.top(); values.pop();
        auto node = operators.top(); operators.pop();

        node->children.push_back(operand);

        values.push(node);
    };
    
    auto addChild = [&]()
    {
        if(values.size() < 2 || IsUnary(operators.top()->expression.first))
        {
            addUnaryChild();
            return;
        }

        if(values.size() % 2 == 1 && GetOperatorPriority(operators.top()->expression.first) == 2)
        {
            auto none = values.top(); values.pop();
            values.top()->children.push_back(none);
            return;
        }
        
        auto right = values.top(); values.pop();
        auto left = values.top(); values.pop();
        auto node = operators.top(); operators.pop();

        node->children.push_back(left);
        node->children.push_back(right);

        values.push(node);
    };

    auto collapseStack = [&]()
    {
        while(values.size() > 1)
        {
            auto node = values.top(); values.pop();
            values.top()->children.push_back(node);
        }
    };

    for(auto i = tokens.begin(); i < tokens.end(); i++)
    {
        /*if(operators.size() == 1)
            bottomOperator = operators.top();*/
            
        switch(i->first)
        {
        case Lexer::Lexeme::ReservedWord:
        case Lexer::Lexeme::Word:
        case Lexer::Lexeme::String:
        case Lexer::Lexeme::Int:
        case Lexer::Lexeme::Float:
        case Lexer::Lexeme::Bool:
            values.push(std::make_shared<Node>(*i)); break;

        case Lexer::Lexeme::BraceOpen:
            //if((i + 1)->first == Lexer::Lexeme::BraceClose)
            {
                /*if(operators.size() > 1 && values.size() % 2 != 1)
                    values.top()->children.push_back(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
                else*/ if((i + 1)->first == Lexer::Lexeme::BraceClose)
                    values.push(std::make_shared<Node>(std::make_pair(Lexer::Lexeme::None, "")));
            }
            operators.push(std::make_shared<Node>(*i));
            break;
            
        case Lexer::Lexeme::BraceClose: // needs improvements
        {
            while(!operators.empty() && operators.top()->expression.first != Lexer::Lexeme::BraceOpen)
                addChild();
            operators.pop();
            
            /*if(bottomOperator)
                if(GetOperatorPriority(bottomOperator->expression.first) == 2)
                    break;*/

            if(!operators.empty())
                if(operators.top()->expression.first == Lexer::Lexeme::Colon)
                    break;

            if(values.size() > 2)
            {
                auto top = values.top(); values.pop();
                values.top()->children.push_back(top);
            }
            break;
        }

        case Lexer::Lexeme::Pow:
        case Lexer::Lexeme::Mod:
        case Lexer::Lexeme::AddAssign:
        case Lexer::Lexeme::SubtractAssign:
        case Lexer::Lexeme::MultiplyAssign:
        case Lexer::Lexeme::DivideAssign:
        case Lexer::Lexeme::Comma:
        case Lexer::Lexeme::Dot:
        case Lexer::Lexeme::IsLess:
        case Lexer::Lexeme::IsGreater:
        case Lexer::Lexeme::IsLessOrEqual:
        case Lexer::Lexeme::IsGreaterOrEqual:
        case Lexer::Lexeme::IsEqual:
        case Lexer::Lexeme::IsNotEqual:
        case Lexer::Lexeme::Not:
        case Lexer::Lexeme::And:
        case Lexer::Lexeme::Or:
        case Lexer::Lexeme::BitwiseAnd:
        case Lexer::Lexeme::BitwiseOr:
        case Lexer::Lexeme::LeftShift:
        case Lexer::Lexeme::RightShift:
        case Lexer::Lexeme::Equal:
        case Lexer::Lexeme::UnaryMinus:
        case Lexer::Lexeme::Decrement:
        case Lexer::Lexeme::Increment:
        case Lexer::Lexeme::Minus:
        case Lexer::Lexeme::Plus:
        case Lexer::Lexeme::Multiply:
        case Lexer::Lexeme::Divide:
        case Lexer::Lexeme::InRange:
        case Lexer::Lexeme::Colon:
            while(!operators.empty() && GetOperatorPriority(operators.top()->expression.first) >= GetOperatorPriority(i->first))
                addChild();
            operators.push(std::make_shared<Node>(*i));
            break;

        case Lexer::Lexeme::CurlyBraceClose:
            curlyBraces.pop(); break;

        case Lexer::Lexeme::Arrow:
        case Lexer::Lexeme::CurlyBraceOpen:
        case Lexer::Lexeme::Semicolon:
            while(!operators.empty())
                addChild();

            collapseStack();
            
            if(curlyBraces.size() == 0)
                root->children.push_back(values.top());
            else
                curlyBraces.top()->children.push_back(values.top());

            if(curlyBraces.size() > 0)
                if(curlyBraces.top()->expression.first == Lexer::Lexeme::Arrow)
                    curlyBraces.pop();

            if(i->first == Lexer::Lexeme::CurlyBraceOpen || 
               i->first == Lexer::Lexeme::Arrow)
            {
                curlyBraces.push(std::make_shared<Node>(*i));
                values.top()->children.push_back(curlyBraces.top());
            }

            values.pop();

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

int AST::GetOperatorPriority(Lexer::Lexeme lexeme)
{
    switch(lexeme)
    {
    case Lexer::Lexeme::Comma: return 3;
    case Lexer::Lexeme::Dot: return 9;
    case Lexer::Lexeme::IsLess: return 4;
    case Lexer::Lexeme::IsGreater: return 4;
    case Lexer::Lexeme::IsLessOrEqual: return 4;
    case Lexer::Lexeme::IsGreaterOrEqual: return 4;
    case Lexer::Lexeme::IsEqual: return 4;
    case Lexer::Lexeme::IsNotEqual: return 4;
    case Lexer::Lexeme::Not: return 7;
    case Lexer::Lexeme::And: return 1;
    case Lexer::Lexeme::Or: return 1;
    case Lexer::Lexeme::BitwiseAnd: return 4;
    case Lexer::Lexeme::BitwiseOr: return 4;
    case Lexer::Lexeme::LeftShift: return 5;
    case Lexer::Lexeme::RightShift: return 5;
    case Lexer::Lexeme::Mod: return 5;
    case Lexer::Lexeme::InRange: return 1;
    case Lexer::Lexeme::Colon: return 6;
    case Lexer::Lexeme::Equal: return 2;
    case Lexer::Lexeme::UnaryMinus: return 7;
    case Lexer::Lexeme::Decrement: return 8;
    case Lexer::Lexeme::Increment: return 8;
    case Lexer::Lexeme::Minus: return 4;
    case Lexer::Lexeme::Plus: return 4;
    case Lexer::Lexeme::Multiply: return 5;
    case Lexer::Lexeme::Divide: return 5;
    case Lexer::Lexeme::Pow: return 6;
    case Lexer::Lexeme::AddAssign: return 2;
    case Lexer::Lexeme::SubtractAssign: return 2;
    case Lexer::Lexeme::MultiplyAssign: return 2;
    case Lexer::Lexeme::DivideAssign: return 2;
    
    default: return 0;
    }
}

bool AST::IsUnary(Lexer::Lexeme lexeme)
{
    switch(lexeme)
    {
    case Lexer::Lexeme::Not:
    case Lexer::Lexeme::Decrement:
    case Lexer::Lexeme::Increment:
    case Lexer::Lexeme::UnaryMinus: return true;

    default: return false;
    }
}

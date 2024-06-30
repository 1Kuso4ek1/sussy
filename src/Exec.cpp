#include <Exec.hpp>

AST::NodeList GetCommaSeparatedNodes(std::shared_ptr<AST::Node> node, AST::NodeList found)
{
    if(found.empty() && node->expression.first != Lexer::Lexeme::Comma && node->expression.first != Lexer::Lexeme::None)
        found.push_back(node);
    for(auto& i : node->children)
        if(i->expression.first != Lexer::Lexeme::Comma && i->expression.first != Lexer::Lexeme::None)
            found.push_back(i);
        else found = GetCommaSeparatedNodes(i, found);

    return found;
}

Lexer::Token GetReturn(std::shared_ptr<AST::Node> node, VarMap& vars)
{
    if(!execNext)
    {
        execNext = true;
        return { Lexer::Lexeme::None, "" };
    }

    if(node->children.empty())
    {
        if(node->expression.first == Lexer::Lexeme::Word)
        {
            auto it = vars.find(node->expression.second);
            if(it == vars.end())
                vars[node->expression.second] = std::make_shared<Variable>();
            else return it->second->GetData();
        }
        return node->expression;
    }

    Lexer::Token leftRet;
    Lexer::Token rightRet;

    if(node->children.size() == 2)
    {
        leftRet = GetReturn(node->children[0], vars);
        rightRet = GetReturn(node->children[1], vars);
    }

    switch(node->expression.first)
    {
    case Lexer::Lexeme::ReservedWord:
    {
        if(node->children[0]->expression.first == Lexer::Lexeme::BraceOpen)
        {
            std::vector<Lexer::Token> args;
            for(auto i : node->children[0]->children)
                if(i->expression.first != Lexer::Lexeme::None && 
                   i->expression.first != Lexer::Lexeme::CurlyBraceOpen)
                    args.push_back(GetReturn(i, vars));

            auto c = node->children[0]->children.back()->children;

            if(node->expression.second == "if")
            {
                if(std::find(args.begin(), args.end(), Lexer::Token(Lexer::Lexeme::Bool, "false")) == args.end())
                    for(auto i : c)
                        GetReturn(i, vars);
                break;
            }

            if(node->expression.second == "while")
            {
                while(std::find(args.begin(), args.end(), Lexer::Token(Lexer::Lexeme::Bool, "false")) == args.end())
                {
                    for(auto i : c)
                        GetReturn(i, vars);
                    args.clear();
                    for(auto i : node->children[0]->children)
                        if(i->expression.first != Lexer::Lexeme::None && 
                           i->expression.first != Lexer::Lexeme::CurlyBraceOpen)
                            args.push_back(GetReturn(i, vars));
                }
            }

            return { Lexer::Lexeme::None, "" };
        }
        return node->expression;
    }

    case Lexer::Lexeme::Int:
    case Lexer::Lexeme::Float:
    case Lexer::Lexeme::String:
    case Lexer::Lexeme::Word:
    {
        if(!node->children.empty())
        {
            auto it = functions.find(node->expression.second);
            if(it == functions.end()) return node->expression;

            auto nodes = GetCommaSeparatedNodes(node);
            std::vector<Lexer::Token> args;
            for(auto& i : nodes)
                args.push_back(GetReturn(i, vars));

            it->second.SetArgs(args, vars);

            if(!it->second.GetBody())
                return it->second.Execute();

            Lexer::Token ret;
            auto c = it->second.GetBody()->children;
            for(auto i = c.begin(); i < c.end(); i++)
                ret = GetReturn(*i, it->second.GetLocalVariables());

            return ret;
        }
        return node->expression;
    }

    case Lexer::Lexeme::Equal:
    {
        if(node->children.size() > 2)
            if(node->children[2]->expression.first == Lexer::Lexeme::CurlyBraceOpen)
            {
                AST::NodeList args;
                std::shared_ptr<AST::Node> body;
            
                args = GetCommaSeparatedNodes(node->children[1]);
                
                body = node->children[2];

                auto it = vars.find(node->children[0]->expression.second);
                if(it != vars.end())
                    vars.erase(it);
                functions[node->children[0]->expression.second] = Function(args, body);

                return node->children[0]->expression;
            }

        auto it = vars.find(node->children[0]->expression.second);
        if(it != vars.end())
        {
            *(it->second) = std::make_pair(rightRet.first, rightRet.second);
            return it->second->GetData();
        }
        return rightRet;
    }

    case Lexer::Lexeme::IsEqual: return IsEqual(leftRet, rightRet);
    case Lexer::Lexeme::IsLess: return IsLess(leftRet, rightRet);
    case Lexer::Lexeme::IsGreater: return IsGreater(leftRet, rightRet);
    case Lexer::Lexeme::IsLessOrEqual: return IsLessOrEqual(leftRet, rightRet);
    case Lexer::Lexeme::IsGreaterOrEqual: return IsGreaterOrEqual(leftRet, rightRet);
    case Lexer::Lexeme::Plus: return Add(leftRet, rightRet);
    case Lexer::Lexeme::Minus: return Subtract(leftRet, rightRet);
    case Lexer::Lexeme::Multiply: return Multiply(leftRet, rightRet);
    case Lexer::Lexeme::Divide: return Divide(leftRet, rightRet);
    }

    return { Lexer::Lexeme::None, "" };
}

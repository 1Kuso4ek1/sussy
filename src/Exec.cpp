#include <Exec.hpp>

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

    if(node->expression.first != Lexer::Lexeme::BraceOpen && node->children.size() == 2)
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

    case Lexer::Lexeme::Word:
    {
        if(node->children[0]->expression.first == Lexer::Lexeme::BraceOpen)
        {
            auto it = functions.find(node->expression.second);
            if(it == functions.end()) return node->expression;

            std::vector<Lexer::Token> args;
            for(auto i : node->children[0]->children)
                if(i->expression.first != Lexer::Lexeme::None)
                    args.push_back(GetReturn(i, vars));

            it->second.SetArgs(args, vars);

            if(!it->second.GetBody())
                return it->second.Execute();

            Lexer::Token ret;
            auto c = it->second.GetBody()->children;
            for(auto i = c.begin(); i < c.end() - 1/* && ret.first == Lexer::Lexeme::None*/; i++)
                ret = GetReturn(*i, it->second.GetLocalVariables());

            return ret;
        }
        return node->expression;
    }

    case Lexer::Lexeme::Equal:
    {
        if(node->children[1]->children.size() > 0)
            if(node->children[1]->children.back()->expression.first == Lexer::Lexeme::CurlyBraceOpen)
            {
                std::vector<std::shared_ptr<AST::Node>> args;
                std::shared_ptr<AST::Node> body;
            
                for(auto i : node->children[1]->children)
                    if(i->expression.first != Lexer::Lexeme::None &&
                       i->expression.first != Lexer::Lexeme::CurlyBraceOpen)
                        args.push_back(i);
                
                body = node->children[1]->children.back();

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

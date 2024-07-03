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

    Lexer::Token ret = { Lexer::Lexeme::None, "" };

    static bool skipElse = false;

    auto assign = [&](Lexer::Token data)
    {
        auto it = vars.find(node->children[0]->expression.second);
        if(it != vars.end())
        {
            *(it->second) = std::make_pair(data.first, data.second);
            return it->second->GetData();
        }
        return data;
    };

    if(node->children.size() == 2) // Improve
    {
        leftRet = GetReturn(node->children[0], vars);
        rightRet = GetReturn(node->children[1], vars);
    }

    switch(node->expression.first)
    {
    case Lexer::Lexeme::ReservedWord:
    {
        if(node->children.size() > 0)
        {
            AST::NodeList nodes;
            std::vector<Lexer::Token> args;

            if(node->children.size() > 1)
            {
                nodes = GetCommaSeparatedNodes(node->children[0]);
                for(auto& i : nodes)
                    args.push_back(GetReturn(i, vars));
            }

            if(node->expression.second == "if" || (node->expression.second == "elseif" && !skipElse))
            {
                skipElse = false;

                if(std::find(args.begin(), args.end(), Lexer::Token(Lexer::Lexeme::Bool, "false")) == args.end())
                {
                    skipElse = true;

                    for(auto i : node->children[1]->children)
                        ret = GetReturn(i, vars);
                }
            }
            else if(node->expression.second == "else" && !skipElse)
            {
                skipElse = true;

                for(auto i : node->children[0]->children)
                    ret = GetReturn(i, vars);
            }
            else if(node->expression.second == "while")
            {
                while(std::find(args.begin(), args.end(), Lexer::Token(Lexer::Lexeme::Bool, "false")) == args.end())
                {
                    for(auto i : node->children[1]->children)
                        ret = GetReturn(i, vars);
                    args.clear();
                    args.push_back(GetReturn(node->children[0], vars));
                }
            }

            return ret;
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

            auto nodes = GetCommaSeparatedNodes(node->children[0]);
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

        return assign(rightRet);
    }

    case Lexer::Lexeme::AddAssign: return assign(Add(leftRet, rightRet));
    case Lexer::Lexeme::SubtractAssign: return assign(Subtract(leftRet, rightRet));
    case Lexer::Lexeme::MultiplyAssign: return assign(Multiply(leftRet, rightRet));
    case Lexer::Lexeme::DivideAssign: return assign(Divide(leftRet, rightRet));

    case Lexer::Lexeme::IsEqual: return IsEqual(leftRet, rightRet);
    case Lexer::Lexeme::IsLess: return IsLess(leftRet, rightRet);
    case Lexer::Lexeme::IsGreater: return IsGreater(leftRet, rightRet);
    case Lexer::Lexeme::IsLessOrEqual: return IsLessOrEqual(leftRet, rightRet);
    case Lexer::Lexeme::IsGreaterOrEqual: return IsGreaterOrEqual(leftRet, rightRet);
    case Lexer::Lexeme::And: return And(leftRet, rightRet);
    case Lexer::Lexeme::Or: return Or(leftRet, rightRet);
    case Lexer::Lexeme::BitwiseAnd: return BitwiseAnd(leftRet, rightRet);
    case Lexer::Lexeme::BitwiseOr: return BitwiseOr(leftRet, rightRet);
    case Lexer::Lexeme::LeftShift: return LeftShift(leftRet, rightRet);
    case Lexer::Lexeme::RightShift: return RightShift(leftRet, rightRet);

    case Lexer::Lexeme::Plus: return Add(leftRet, rightRet);
    case Lexer::Lexeme::Minus: return Subtract(leftRet, rightRet);
    case Lexer::Lexeme::Multiply: return Multiply(leftRet, rightRet);
    case Lexer::Lexeme::Divide: return Divide(leftRet, rightRet);
    case Lexer::Lexeme::Pow: return Pow(leftRet, rightRet);
    }

    return { Lexer::Lexeme::None, "" };
}

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

std::shared_ptr<Variable> GetReturn(std::shared_ptr<AST::Node> node, VarMap& vars)
{
    if(node->children.empty())
    {
        if(node->expression.first == Lexer::Lexeme::Word)
        {
            auto it = vars.find(node->expression.second);
            if(it == vars.end())
                vars[node->expression.second] = std::make_shared<Variable>();
            else return it->second;
        }
        if(node->expression.first != Lexer::Lexeme::ReservedWord)
            return std::make_shared<Variable>(node->expression);
    }

    std::shared_ptr<Variable> leftRet;
    std::shared_ptr<Variable> rightRet;

    std::shared_ptr<Variable> ret = std::make_shared<Variable>(Lexer::Token(Lexer::Lexeme::None, ""));

    static bool skipElse = false;
    static bool breakBlock = false;
    static bool continueBlock = false;
    static bool returnValue = false;

    auto assign = [&](std::shared_ptr<Variable> data)
    {
        auto it = vars.find(node->children[0]->expression.second);
        if(it != vars.end())
        {
            *(it->second) = data;
            return it->second;
        }
        return data;
    };

    if(node->children.size() == 2 && node->expression.first != Lexer::Lexeme::ReservedWord) // Improve
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
            std::vector<std::shared_ptr<Variable>> args;

            if(node->children.size() > 1)
            {
                nodes = GetCommaSeparatedNodes(node->children[0]);
                for(auto& i : nodes)
                    args.push_back(GetReturn(i, vars));
            }

            if(node->expression.second == "if" || (node->expression.second == "elseif" && !skipElse))
            {
                skipElse = false;

                auto it = std::find_if(args.begin(), args.end(), [&](const std::shared_ptr<Variable>& a) { return *a == Lexer::Token(Lexer::Lexeme::Bool, "false"); });

                if(it == args.end())
                {
                    skipElse = true;

                    for(auto i : node->children[1]->children)
                    {
                        ret = GetReturn(i, vars);
                        if(returnValue) break;
                    }
                }
            }
            else if(node->expression.second == "else" && !skipElse)
            {
                skipElse = true;

                for(auto i : node->children[0]->children)
                {
                    ret = GetReturn(i, vars);
                    if(returnValue) break;
                }
            }
            else if(node->expression.second == "switch")
            {
                breakBlock = continueBlock = false;

                auto switchVar = vars.find(node->children[0]->expression.second);
                
                for(auto i = node->children[1]->children.begin(); i < node->children[1]->children.end(); i++)
                {
                    if((*i)->expression.first == Lexer::Lexeme::ReservedWord && ((*i)->expression.second == "case" || (*i)->expression.second == "default"))
                    {
                        if(IsEqual(switchVar->second->GetData(), GetReturn((*i)->children[0], vars)->GetData()).second == "true" || (*i)->expression.second == "default")
                        {
                            for(auto j : (*i)->children[1]->children)
                            {
                                ret = GetReturn(j, vars);
                                if(returnValue) break;
                            }
                            break;
                        }
                    }
                }
            }
            else if(node->expression.second == "while")
            {
                breakBlock = continueBlock = false;

                while(std::find_if(args.begin(), args.end(), [&](const std::shared_ptr<Variable>& a) { return *a == Lexer::Token(Lexer::Lexeme::Bool, "false"); }) == args.end())
                {
                    for(auto i : node->children[1]->children)
                    {
                        if(breakBlock || continueBlock) break;
                        else ret = GetReturn(i, vars);

                        if(returnValue) break;
                    }
                    
                    if(breakBlock || returnValue) break;
                    continueBlock = false;

                    args.clear();
                    args.push_back(GetReturn(node->children[0], vars));
                }
            }
            else if(node->expression.second == "for")
            {
                breakBlock = continueBlock = false;

                auto iteratorVar = std::find_if(vars.begin(), vars.end(), [&](const auto& a) { return a.second == args[0]; });
                int start = stoi(args[2]->GetData().second);
                int end = stoi(args.back()->GetData().second);

                for(int i = start; (start < end ? i < end : i > end); i += (start < end ? 1 : -1))
                {
                    *iteratorVar->second = std::make_shared<Variable>(Lexer::Token(Lexer::Lexeme::Int, std::to_string(i)));

                    for(auto i : node->children[1]->children)
                    {
                        if(breakBlock || continueBlock) break;
                        else ret = GetReturn(i, vars);

                        if(returnValue) break;
                    }
                    
                    if(breakBlock || returnValue) break;
                    continueBlock = false;
                }

                vars.erase(iteratorVar);
            }
            else if(node->expression.second == "return")
            {
                returnValue = true;
                if(node->children.size() > 0)
                    ret = GetReturn(node->children[0], vars);
            }

            return ret;
        }
        
        breakBlock = (node->expression.second == "break");
        continueBlock = (node->expression.second == "continue");

        return std::make_shared<Variable>(node->expression);
    }

    case Lexer::Lexeme::Int:
    case Lexer::Lexeme::Float:
    case Lexer::Lexeme::String:
    case Lexer::Lexeme::Word:
    {
        if(!node->children.empty())
        {
            auto it = functions.find(node->expression.second);
            if(it == functions.end()) return std::make_shared<Variable>(node->expression);

            auto nodes = GetCommaSeparatedNodes(node->children[0]);
            std::vector<std::shared_ptr<Variable>> args;
            for(auto& i : nodes)
                args.push_back(GetReturn(i, vars));

            it->second.SetArgs(args, vars);

            if(!it->second.GetBody())
                return std::make_shared<Variable>(it->second.Execute());

            returnValue = false;

            std::shared_ptr<Variable> ret;
            auto c = it->second.GetBody()->children;
            for(auto i = c.begin(); i < c.end() && !returnValue; i++)
                ret = GetReturn(*i, it->second.GetLocalVariables());

            returnValue = false;

            return ret;
        }
        return std::make_shared<Variable>(node->expression);
    }

    case Lexer::Lexeme::InRange: return assign(rightRet); return leftRet;
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

                return std::make_shared<Variable>(node->children[0]->expression);
            }
        
        if(node->children[0]->expression.first == Lexer::Lexeme::Colon)
        {
            auto var = vars.find(node->children[0]->children[0]->expression.second);
            if(var != vars.end())
                if(var->second->GetType() == Variable::VariableType::Array)
                {
                    auto value = GetReturn(node->children[1], vars);
                    auto index = GetReturn(node->children[0]->children[1], vars);
                    auto element = std::make_shared<Variable>(value->GetData());
                    var->second->SetElement(stoi(index->GetData().second), element);

                    return element;
                }

            return std::make_shared<Variable>(node->expression);
        }
        else if(node->children[1]->expression.first == Lexer::Lexeme::Colon)
        {
            auto array = std::make_shared<Variable>(std::make_pair(Lexer::Lexeme::Word, node->children[0]->expression.second), Variable::VariableType::Array, stoi(GetReturn(node->children[1]->children[0], vars)->GetData().second));
            array->Fill(GetReturn(node->children[1]->children[1], vars)->GetData());
            vars[node->children[0]->expression.second] = array;

            return array->GetElement(0);
        }

        return assign(rightRet);
    }

    case Lexer::Lexeme::Colon:
    {
        auto it = vars.find(node->children[0]->expression.second);
        if(it != vars.end())
        {
            if(it->second->GetType() == Variable::VariableType::Array)
                return it->second->GetElement(stoi(GetReturn(node->children[1], vars)->GetData().second));
        }

        return std::make_shared<Variable>(Index(leftRet->GetData(), rightRet->GetData()));
    }

    case Lexer::Lexeme::AddAssign: return assign(std::make_shared<Variable>(Add(leftRet->GetData(), rightRet->GetData())));
    case Lexer::Lexeme::SubtractAssign: return assign(std::make_shared<Variable>(Subtract(leftRet->GetData(), rightRet->GetData())));
    case Lexer::Lexeme::MultiplyAssign: return assign(std::make_shared<Variable>(Multiply(leftRet->GetData(), rightRet->GetData())));
    case Lexer::Lexeme::DivideAssign: return assign(std::make_shared<Variable>(Divide(leftRet->GetData(), rightRet->GetData())));

    case Lexer::Lexeme::IsEqual: return std::make_shared<Variable>(IsEqual(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::IsLess: return std::make_shared<Variable>(IsLess(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::IsGreater: return std::make_shared<Variable>(IsGreater(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::IsLessOrEqual: return std::make_shared<Variable>(IsLessOrEqual(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::IsGreaterOrEqual: return std::make_shared<Variable>(IsGreaterOrEqual(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::And: return std::make_shared<Variable>(And(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::Or: return std::make_shared<Variable>(Or(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::BitwiseAnd: return std::make_shared<Variable>(BitwiseAnd(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::BitwiseOr: return std::make_shared<Variable>(BitwiseOr(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::LeftShift: return std::make_shared<Variable>(LeftShift(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::RightShift: return std::make_shared<Variable>(RightShift(leftRet->GetData(), rightRet->GetData()));

    case Lexer::Lexeme::Plus: return std::make_shared<Variable>(Add(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::Minus: return std::make_shared<Variable>(Subtract(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::Multiply: return std::make_shared<Variable>(Multiply(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::Divide: return std::make_shared<Variable>(Divide(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::Pow: return std::make_shared<Variable>(Pow(leftRet->GetData(), rightRet->GetData()));
    case Lexer::Lexeme::Mod: return std::make_shared<Variable>(Mod(leftRet->GetData(), rightRet->GetData()));
    }
  
    return std::make_shared<Variable>(Lexer::Token(Lexer::Lexeme::None, ""));
}

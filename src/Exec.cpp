#include <Exec.hpp>

AST::NodeList GetCommaSeparatedNodes(std::shared_ptr<AST::Node> node, AST::NodeList found)
{
    if(found.empty() && node->expression.first != Lexer::Lexeme::Comma && node->expression.first != Lexer::Lexeme::None)
        found.push_back(node);

    //else if(node->expression.first == Lexer::Lexeme::Comma || node->expression.first == Lexer::Lexeme::None)
    for(auto& i : node->children)
        if(i->expression.first != Lexer::Lexeme::Comma && i->expression.first != Lexer::Lexeme::None)
            found.push_back(i);
        else found = GetCommaSeparatedNodes(i, found);

    return found;
}

std::shared_ptr<Variable> GetReturn(std::shared_ptr<AST::Node> node, std::vector<VarMap>& scopes)
{
    auto findVariableByName = [&](std::string name) -> std::shared_ptr<Variable>
    {
        for(auto i = scopes.rbegin(); i < scopes.rend(); i++)
        {
            auto it = i->find(name);
            if(it != i->end())
                return it->second;
        }

        return nullptr;
    };

    auto findVariableByPointer = [&](std::shared_ptr<Variable> var) -> std::shared_ptr<Variable>
    {
        for(auto i = scopes.rbegin(); i < scopes.rend(); i++)
        {
            auto it = std::find_if(i->begin(), i->end(), [&](const auto& a) { return *a.second == var; });
            if(it != i->end())
                return it->second;
        }

        return nullptr;
    };

    if(node->children.empty())
    {
        if(node->expression.first == Lexer::Lexeme::Word)
        {
            auto var = findVariableByName(node->expression.second);
            if(!var)
            {
                scopes.back()[node->expression.second] = std::make_shared<Variable>();
                return scopes.back()[node->expression.second];
            }
            else return var;
        }
        if(node->expression.first != Lexer::Lexeme::ReservedWord)
            return std::make_shared<Variable>(node->expression);
        return nullptr;
    }

    std::shared_ptr<Variable> leftRet;
    std::shared_ptr<Variable> rightRet;

    std::shared_ptr<Variable> ret;

    static bool skipElse = false;
    static bool breakBlock = false;
    static bool continueBlock = false;
    static bool returnValue = false;

    auto assign = [&](std::shared_ptr<Variable> data)
    {
        auto var = findVariableByName(node->children[0]->expression.second);
        if(var)
        {
            *(var) = data;
            return var;
        }
        return data;
    };

    if(node->children.size() == 2 && node->expression.first != Lexer::Lexeme::ReservedWord) // Improve
    {
        leftRet = GetReturn(node->children[0], scopes);
        rightRet = GetReturn(node->children[1], scopes);
    }

    switch(node->expression.first)
    {
    case Lexer::Lexeme::ReservedWord:
    {
        if(node->children.size() > 0)
        {
            AST::NodeList nodes;
            std::vector<std::shared_ptr<Variable>> args;

            scopes.emplace_back();

            if(node->children.size() > 1)
            {
                nodes = GetCommaSeparatedNodes(node->children[0]);
                for(auto& i : nodes)
                    args.push_back(GetReturn(i, scopes));
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
                        ret = GetReturn(i, scopes);
                        if(returnValue) break;
                    }
                }
            }
            else if(node->expression.second == "else" && !skipElse)
            {
                skipElse = true;

                for(auto i : node->children[0]->children)
                {
                    ret = GetReturn(i, scopes);
                    if(returnValue) break;
                }
            }
            else if(node->expression.second == "switch")
            {
                breakBlock = continueBlock = false;

                auto switchVar = findVariableByName(node->children[0]->expression.second);
                
                for(auto i = node->children[1]->children.begin(); i < node->children[1]->children.end(); i++)
                {
                    if((*i)->expression.first == Lexer::Lexeme::ReservedWord && ((*i)->expression.second == "case" || (*i)->expression.second == "default"))
                    {
                        if(IsEqual(switchVar->GetData(), GetReturn((*i)->children[0], scopes)->GetData()).second == "true" || (*i)->expression.second == "default")
                        {
                            for(auto j : (*i)->children[1]->children)
                            {
                                ret = GetReturn(j, scopes);
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
                        else ret = GetReturn(i, scopes);

                        if(returnValue) break;
                    }
                    
                    if(breakBlock || returnValue) break;
                    continueBlock = false;

                    args.clear();
                    args.push_back(GetReturn(node->children[0], scopes));
                }
            }
            else if(node->expression.second == "for")
            {
                breakBlock = continueBlock = false;

                auto iteratorVar = findVariableByPointer(args[0]);
                int start = stoi(args[2]->GetData().second);
                int end = stoi(args.back()->GetData().second);

                for(int i = start; (start < end ? i < end : i > end); i += (start < end ? 1 : -1))
                {
                    *iteratorVar = Lexer::Token(Lexer::Lexeme::Int, std::to_string(i));

                    for(auto i : node->children[1]->children)
                    {
                        if(breakBlock || continueBlock) break;
                        else ret = GetReturn(i, scopes);

                        if(returnValue) break;
                    }
                    
                    if(breakBlock || returnValue) break;
                    continueBlock = false;
                }
            }
            else if(node->expression.second == "return")
            {
                returnValue = true;
                if(node->children.size() > 0)
                    ret = GetReturn(node->children[0], scopes);
            }

            scopes.pop_back();

            return ret;
        }
        
        breakBlock = (node->expression.second == "break");
        continueBlock = (node->expression.second == "continue");

        return nullptr;
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
                args.push_back(GetReturn(i, scopes));

            it->second.SetArgs(args, scopes.back());

            if(!it->second.GetBody())
                return std::make_shared<Variable>(it->second.Execute());

            returnValue = false;

            scopes.push_back(it->second.GetLocalVariables());

            std::shared_ptr<Variable> ret;
            auto c = it->second.GetBody()->children;
            for(auto i = c.begin(); i < c.end() && !returnValue; i++)
                ret = GetReturn(*i, scopes);

            scopes.pop_back();

            returnValue = false;

            return ret;
        }
        
        return std::make_shared<Variable>(node->expression);
    }

    case Lexer::Lexeme::InRange: assign(rightRet); return leftRet;
    case Lexer::Lexeme::Equal:
    {
        if(node->children.size() > 2)
            if(node->children[2]->expression.first == Lexer::Lexeme::CurlyBraceOpen)
            {
                AST::NodeList args;
                std::shared_ptr<AST::Node> body;
            
                args = GetCommaSeparatedNodes(node->children[1]);
                
                body = node->children[2];

                auto it = scopes.back().find(node->children[0]->expression.second);
                if(it != scopes.back().end())
                    scopes.back().erase(it);
                functions[node->children[0]->expression.second] = Function(args, body);

                return nullptr;
            }
        
        if(node->children[0]->expression.first == Lexer::Lexeme::Colon)
        {
            auto var = findVariableByName(node->children[0]->children[0]->expression.second);
            if(var)
                if(var->GetType() == Variable::VariableType::Array)
                {
                    auto value = GetReturn(node->children[1], scopes);
                    auto index = GetReturn(node->children[0]->children[1], scopes);
                    auto element = std::make_shared<Variable>(value->GetData());
                    var->SetElement(stoi(index->GetData().second), element);

                    return element;
                }

            return std::make_shared<Variable>(node->expression);
        }
        else if(node->children[1]->expression.first == Lexer::Lexeme::Colon &&
                node->children[1]->children[0]->expression.first == Lexer::Lexeme::Int)
        {
            auto array = std::make_shared<Variable>(std::make_pair(Lexer::Lexeme::Word, node->children[0]->expression.second), Variable::VariableType::Array, stoi(GetReturn(node->children[1]->children[0], scopes)->GetData().second));
            array->Fill(GetReturn(node->children[1]->children[1], scopes)->GetData());
            scopes.back()[node->children[0]->expression.second] = array;

            return array->GetElement(0);
        }
        else if(node->children[0]->expression.first == Lexer::Lexeme::Comma)
        {
            // make it a separate function
            AST::NodeList nodes;
            nodes = GetCommaSeparatedNodes(node->children[1]);
            std::vector<std::shared_ptr<Variable>> args;
            for(auto& i : nodes)
                args.push_back(GetReturn(i, scopes));

            auto var = findVariableByName(node->children[0]->expression.second);
            if(var)
                if(var->GetType() == Variable::VariableType::Array)
                {
                    for(int i = 0; i < args.size(); i++)
                        var->SetElement(i, args[i]);
                    return var->GetElement(args.size() - 1);
                }

            return std::make_shared<Variable>(node->expression);
        }

        return assign(rightRet);
    }

    case Lexer::Lexeme::Colon:
    {
        auto var = findVariableByName(node->children[0]->expression.second);
        if(var)
        {
            if(var->GetType() == Variable::VariableType::Array)
                return var->GetElement(stoi(GetReturn(node->children[1], scopes)->GetData().second));
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

// Twice as SLOW
void GetReturnIterative(std::shared_ptr<AST::Node> root, std::vector<VarMap>& scopes)
{
    std::stack<std::shared_ptr<AST::Node>> stack;
    std::stack<std::shared_ptr<AST::Node>> waitingForArgs;
    std::stack<std::shared_ptr<Variable>> valueStack;

    stack.push(root);

    bool skipElse = false;
    bool breakBlock = false;
    bool continueBlock = false;
    bool returnValue = false;
    bool discardValue = false;

    auto getBinaryOperatorArgs = [&](std::shared_ptr<AST::Node> node)
    {
        if(node->expression.first != Lexer::Lexeme::ReservedWord) // Improve
        {
            stack.push(node->children[1]);
            stack.push(node->children[0]);
        }
    };

    auto execBinaryOperator = [&](std::shared_ptr<AST::Node> node, std::function<Lexer::Token(Lexer::Token, Lexer::Token)> binaryOperator)
    {
        std::shared_ptr<AST::Node> top = nullptr;
        if(!waitingForArgs.empty())
            top = waitingForArgs.top();

        if(node == top)
        {
            auto right = valueStack.top(); valueStack.pop();
            auto left = valueStack.top(); valueStack.pop();

            if(!discardValue)
                valueStack.push(std::make_shared<Variable>(binaryOperator(left->GetData(), right->GetData())));
            waitingForArgs.pop();
        }
        else
        {
            stack.push(node);
            getBinaryOperatorArgs(node);
            waitingForArgs.push(node);
        }
    };

    auto findVariableByName = [&](std::string name) -> std::shared_ptr<Variable>
    {
        for(auto i = scopes.rbegin(); i < scopes.rend(); i++)
        {
            auto it = i->find(name);
            if(it != i->end())
                return it->second;
        }

        return nullptr;
    };

    auto assign = [&](std::shared_ptr<AST::Node> node, std::shared_ptr<Variable> data)
    {
        auto var = findVariableByName(node->expression.second);
        if(var)
        {
            *(var) = data;
            return var;
        }
        return data;
    };

    while(!stack.empty())
    {
        auto node = stack.top(); stack.pop();

        if((returnValue || breakBlock || continueBlock) && (node->expression.first != Lexer::Lexeme::CurlyBraceOpen && node->expression.first != Lexer::Lexeme::Arrow)) continue;

        if(node->children.empty())
        {
            if(node->expression.first == Lexer::Lexeme::Word)
            {
                auto var = findVariableByName(node->expression.second);
                if(!var)
                {
                    scopes.back()[node->expression.second] = std::make_shared<Variable>();
                    valueStack.push(scopes.back()[node->expression.second]);
                }
                else valueStack.push(var);

                continue;
            }
            if(node->expression.first != Lexer::Lexeme::ReservedWord)
                valueStack.push(std::make_shared<Variable>(node->expression));

            continue;
        }

        switch(node->expression.first)
        {
        case Lexer::Lexeme::ReservedWord:
        {
            if(node->children.size() > 0)
            {
                std::shared_ptr<AST::Node> top = nullptr;
                if(!waitingForArgs.empty())
                    top = waitingForArgs.top();

                if(node != top && node->children.size() > 0 && node->expression.second != "else")
                {
                    stack.push(node);
                    stack.push(node->children[0]);
                    waitingForArgs.push(node);
                    discardValue = false;
                    break;
                }

                else if(node->expression.second == "if" || (node->expression.second == "elseif" && !skipElse))
                {
                    skipElse = false;

                    if(valueStack.top()->GetData() == Lexer::Token(Lexer::Lexeme::Bool, "true"))
                    {
                        skipElse = true;
                        stack.push(node->children[1]);
                    }

                    valueStack.pop();
                }
                else if(node->expression.second == "else" && !skipElse)
                {
                    skipElse = true;
                    stack.push(node->children[0]);
                    break;
                }
                else if(node->expression.second == "while")
                {
                    breakBlock = continueBlock = false;

                    if(valueStack.top()->GetData() == Lexer::Token(Lexer::Lexeme::Bool, "true"))
                    {
                        stack.push(node);
                        stack.push(node->children[1]);
                    }

                    valueStack.pop();
                }

                if(node->children.size() > 0)
                    waitingForArgs.pop();

                returnValue = node->expression.second == "return";
                breakBlock = node->expression.second == "break";
                continueBlock = node->expression.second == "continue";

                break;
            }
        }
        case Lexer::Lexeme::Equal:
        {
            if(node->children.size() > 2)
                if(node->children[2]->expression.first == Lexer::Lexeme::CurlyBraceOpen ||
                   node->children[2]->expression.first == Lexer::Lexeme::Arrow)
                {
                    AST::NodeList args;
                    std::shared_ptr<AST::Node> body;
                
                    args = GetCommaSeparatedNodes(node->children[1]);
                    
                    body = node->children[2];

                    auto it = scopes.back().find(node->children[0]->expression.second);
                    if(it != scopes.back().end())
                        scopes.back().erase(it);
                    functions[node->children[0]->expression.second] = Function(args, body);

                    continue;
                }

            std::shared_ptr<AST::Node> top = nullptr;
            if(!waitingForArgs.empty())
                top = waitingForArgs.top();

            if(node == top)
            {
                auto value = valueStack.top(); valueStack.pop(); valueStack.pop();
                valueStack.push(assign(node->children[0], value));
                waitingForArgs.pop();
            }
            else
            {
                stack.push(node);
                getBinaryOperatorArgs(node);
                waitingForArgs.push(node);
                discardValue = false;

                continue;
            }

            break;
        }

        case Lexer::Lexeme::Word:
        {
            if(!node->children.empty())
            {
                auto it = functions.find(node->expression.second);
                if(it == functions.end())
                {
                    valueStack.push(std::make_shared<Variable>(node->expression));
                    break;
                }

                std::shared_ptr<AST::Node> top = nullptr;
                if(!waitingForArgs.empty())
                    top = waitingForArgs.top();

                if(/*valueStack.size() < it->second.GetArgsCount() || */node != top)
                {
                    discardValue = false;

                    stack.push(node);
                    auto nodes = GetCommaSeparatedNodes(node->children[0]);
                    for(auto& i : nodes)
                        stack.push(i);

                    waitingForArgs.push(node);
                        
                    continue;
                }

                std::vector<std::shared_ptr<Variable>> args;
                for(int i = 0; i < it->second.GetArgsCount(); i++)
                {
                    args.push_back(valueStack.top());
                    valueStack.pop();
                }

                waitingForArgs.pop();

                it->second.SetArgs(args, scopes.back());

                if(!it->second.GetBody())
                {
                    auto returnValue = std::make_shared<Variable>(it->second.Execute());
                    if(returnValue->GetData().first != Lexer::Lexeme::None && !discardValue)
                        valueStack.push(returnValue);
                    break;
                }
                
                returnValue = false;
                stack.push(it->second.GetBody());
                scopes.push_back(it->second.GetLocalVariables());
                
                break;
            }
            
            valueStack.push(std::make_shared<Variable>(node->expression));
            break;
        }

        case Lexer::Lexeme::Arrow:
        case Lexer::Lexeme::CurlyBraceOpen:
        {
            std::shared_ptr<AST::Node> top = nullptr;
            if(!waitingForArgs.empty())
                top = waitingForArgs.top();

            if(node == top)
            {
                returnValue = discardValue = false;
                //scopes.back() = variables;
                if(scopes.back().size() == 0 && scopes.size() > 2)
                    scopes.pop_back();
                scopes.pop_back();
                waitingForArgs.pop();
                break;
            }

            scopes.emplace_back();
            
            waitingForArgs.push(node);

            discardValue = true;

            stack.push(node);

            for(auto i = node->children.end() - 1; i >= node->children.begin(); i--)
                stack.push(*i);

            break;
        }

        case Lexer::Lexeme::IsEqual: execBinaryOperator(node, IsEqual); break;
        case Lexer::Lexeme::IsLess: execBinaryOperator(node, IsLess); break;
        case Lexer::Lexeme::IsGreater: execBinaryOperator(node, IsGreater); break;
        case Lexer::Lexeme::IsLessOrEqual: execBinaryOperator(node, IsLessOrEqual); break;
        case Lexer::Lexeme::IsGreaterOrEqual: execBinaryOperator(node, IsGreaterOrEqual); break;
        case Lexer::Lexeme::And: execBinaryOperator(node, And); break;
        case Lexer::Lexeme::Or: execBinaryOperator(node, Or); break;
        case Lexer::Lexeme::BitwiseAnd: execBinaryOperator(node, BitwiseAnd); break;
        case Lexer::Lexeme::BitwiseOr: execBinaryOperator(node, BitwiseOr); break;
        case Lexer::Lexeme::LeftShift: execBinaryOperator(node, LeftShift); break;
        case Lexer::Lexeme::RightShift: execBinaryOperator(node, RightShift); break;

        case Lexer::Lexeme::Plus: execBinaryOperator(node, Add); break;
        case Lexer::Lexeme::Minus: execBinaryOperator(node, Subtract); break;
        case Lexer::Lexeme::Multiply: execBinaryOperator(node, Multiply); break;
        case Lexer::Lexeme::Divide: execBinaryOperator(node, Divide); break;
        case Lexer::Lexeme::Pow: execBinaryOperator(node, Pow); break;
        case Lexer::Lexeme::Mod: execBinaryOperator(node, Mod); break;
        
        default:
            break;
        }
    }
}

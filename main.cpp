#include <AST.hpp>
#include <fstream>
#include <iostream>
#include <Variable.hpp>
#include <Function.hpp>

std::string lastAddedVar;

void PrintAST(std::shared_ptr<AST::Node> node, int depth = 0)
{
    if(depth != 0) std::cout << "|";// << std::endl << "|";
    for(int i = 0; i < depth; i++) std::cout << "----";

    switch (node->expression.first)
    {
    case Lexer::Lexeme::None: std::cout << "Expression: { Lexeme::None, "; break;
    case Lexer::Lexeme::BraceOpen: std::cout << "Expression: { Lexeme::BraceOpen, "; break;
    case Lexer::Lexeme::BraceClose: std::cout << "Expression: { Lexeme::BraceClose, "; break;
    case Lexer::Lexeme::CurlyBraceOpen: std::cout << "Expression: { Lexeme::CurlyBraceOpen, "; break;
    case Lexer::Lexeme::CurlyBraceClose: std::cout << "Expression: { Lexeme::CurlyBraceClose, "; break;
    case Lexer::Lexeme::ReservedWord: std::cout << "Expression: { Lexeme::ReservedWord, "; break;
    case Lexer::Lexeme::Word: std::cout << "Expression: { Lexeme::Word, "; break;
    case Lexer::Lexeme::IsEqual: std::cout << "Expression: { Lexeme::IsEqual, "; break;
    case Lexer::Lexeme::IsLess: std::cout << "Expression: { Lexeme::IsLess, "; break;
    case Lexer::Lexeme::IsGreater: std::cout << "Expression: { Lexeme::IsGreater, "; break;
    case Lexer::Lexeme::IsLessOrEqual: std::cout << "Expression: { Lexeme::IsLessOrEqual, "; break;
    case Lexer::Lexeme::IsGreaterOrEqual: std::cout << "Expression: { Lexeme::IsGreaterOrEqual, "; break;
    case Lexer::Lexeme::Equal: std::cout << "Expression: { Lexeme::Equal, "; break;
    case Lexer::Lexeme::Plus: std::cout << "Expression: { Lexeme::Plus, "; break;
    case Lexer::Lexeme::Minus: std::cout << "Expression: { Lexeme::Minus, "; break;
    case Lexer::Lexeme::Multiply: std::cout << "Expression: { Lexeme::Multiply, "; break;
    case Lexer::Lexeme::Divide: std::cout << "Expression: { Lexeme::Divide, "; break;
    case Lexer::Lexeme::Int: std::cout << "Expression: { Lexeme::Int, "; break;
    case Lexer::Lexeme::Float: std::cout << "Expression: { Lexeme::Float, "; break;
    case Lexer::Lexeme::String: std::cout << "Expression: { Lexeme::String, "; break;
    default: break;
    }

    std::cout << node->expression.second << " }" << std::endl;

    for(auto i : node->children)
        PrintAST(i, depth + 1);
}

Lexer::Token GetReturn(std::shared_ptr<AST::Node> node, std::unordered_map<std::string, std::shared_ptr<Variable>>& vars = variables)
{
    if(node->children.empty())
    {
        if(node->expression.first == Lexer::Lexeme::Word)
        {
            auto it = vars.find(node->expression.second);
            if(it == vars.end())
            {
                vars[node->expression.second] = std::make_shared<Variable>();
                lastAddedVar = node->expression.second;
            }
            else return it->second->GetData();
        }
        return node->expression;
    }

    Lexer::Token leftRet;
    Lexer::Token rightRet;

    if(node->children.size() == 2)
    {
        leftRet = GetReturn(node->children[0], vars);
        rightRet = GetReturn(node->children.back(), vars);
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
                    for(auto i = c.begin(); i < c.end(); i++)
                        GetReturn(*i, variables);
                break;
            }

            if(node->expression.second == "while")
            {
                while(std::find(args.begin(), args.end(), Lexer::Token(Lexer::Lexeme::Bool, "false")) == args.end())
                {
                    for(auto i = c.begin(); i < c.end(); i++)
                        GetReturn(*i, variables);
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

            it->second.SetArgs(args, variables);

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
        if(node->children.size() == 3)
        {
            std::vector<std::shared_ptr<AST::Node>> args;
            std::shared_ptr<AST::Node> body;
        
            for(auto i : node->children[1]->children)
                if(i->expression.first != Lexer::Lexeme::None)
                    args.push_back(i);
            
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

int main(int argc, char** argv)
{
    if(argc == 1) return 1;

    std::ifstream file(argv[1]);
	if(!file.is_open())
		return 1;

	std::string code;
	std::copy(std::istreambuf_iterator<char>(file),
			  std::istreambuf_iterator<char>(),
			  std::back_inserter(code));

	Lexer lexer(code);
	auto res = lexer.Result();

    AST ast(res);

    //PrintAST(ast.GetRootNode());

    functions["print"] = Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                                  [](VarMap v) -> Lexer::Token { std::cout << v["value"]->GetData().second; return { Lexer::Lexeme::None, "" }; });
    functions["println"] = Function({ std::make_shared<AST::Node>(std::make_pair(Lexer::Lexeme::Word, "value")) },
                                    [](VarMap v) -> Lexer::Token { std::cout << v["value"]->GetData().second << std::endl; return { Lexer::Lexeme::None, "" }; });

    for(auto i : ast.GetRootNode()->children)
        GetReturn(i);
    
    /*for(auto [name, func] : functions)
        std::cout << name << " " << std::endl;
    for(auto [name, var] : variables)
        std::cout << name << " " << var->GetData().second << std::endl;*/
}

#include <AST.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <Variable.hpp>

std::unordered_map<std::string, Variable> variables;

void PrintAST(std::shared_ptr<AST::Node> node, int depth = 0)
{
    if(depth != 0) std::cout << "|";// << std::endl << "|";
    for(int i = 0; i < depth; i++) std::cout << "----";

    switch (node->expression.first)
    {
    case Lexer::Lexeme::None: std::cout << "Expression: { Lexeme::None, "; break;
    case Lexer::Lexeme::Word: std::cout << "Expression: { Lexeme::Word, "; break;
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

Lexer::Token GetReturn(std::shared_ptr<AST::Node> node)
{
    if(node->children.empty())
    {
        if(node->expression.first == Lexer::Lexeme::Word)
        {
            auto it = variables.find(node->expression.second);
            if(it == variables.end())
                variables[node->expression.second] = Variable();
            else return it->second.GetData();
        }
        return node->expression;
    }

    Lexer::Token leftRet;
    Lexer::Token rightRet;

    if(node->children.size() > 1)
    {
        leftRet = GetReturn(node->children[0]);
        rightRet = GetReturn(node->children.back());
    }

    switch(node->expression.first)
    {
    case Lexer::Lexeme::Equal:
    {
        auto it = variables.find(leftRet.second);
        if(it != variables.end())
        {
            it->second = std::make_pair(rightRet.first, rightRet.second);
            return it->second.GetData();
        }
        return rightRet;
    }

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

    PrintAST(ast.GetRootNode());

    for(auto i : ast.GetRootNode()->children)
        GetReturn(i);
    
    for(auto [name, var] : variables)
        std::cout << name << " " << var.GetData().second << std::endl;
}

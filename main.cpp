#include <fstream>
#include <iostream>
#include <Exec.hpp>

VarMap variables;

void PrintAST(std::shared_ptr<AST::Node> node, int depth = 0)
{
    if(depth != 0) std::cout << "|";// << std::endl << "|";
    for(int i = 0; i < depth; i++) std::cout << "----";

    switch (node->expression.first)
    {
    case Lexer::Lexeme::None: std::cout << "Expression: { Lexeme::None, "; break;
    case Lexer::Lexeme::Comma: std::cout << "Expression: { Lexeme::Comma, "; break;
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

    /*for(auto i : ast.GetRootNode()->children)
        GetReturn(i, variables);*/
    
    /*for(auto [name, func] : functions)
        std::cout << name << " " << std::endl;
    for(auto [name, var] : variables)
        std::cout << name << " " << var->GetData().second << std::endl;*/
}

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
    case Lexer::Lexeme::None: std::cout << "{ Lexeme::None, "; break;
    case Lexer::Lexeme::Comma: std::cout << "{ Lexeme::Comma, "; break;
    case Lexer::Lexeme::BraceOpen: std::cout << "{ Lexeme::BraceOpen, "; break;
    case Lexer::Lexeme::BraceClose: std::cout << "{ Lexeme::BraceClose, "; break;
    case Lexer::Lexeme::CurlyBraceOpen: std::cout << "{ Lexeme::CurlyBraceOpen, "; break;
    case Lexer::Lexeme::CurlyBraceClose: std::cout << "{ Lexeme::CurlyBraceClose, "; break;
    case Lexer::Lexeme::ReservedWord: std::cout << "{ Lexeme::ReservedWord, "; break;
    case Lexer::Lexeme::Word: std::cout << "{ Lexeme::Word, "; break;
    case Lexer::Lexeme::IsEqual: std::cout << "{ Lexeme::IsEqual, "; break;
    case Lexer::Lexeme::IsLess: std::cout << "{ Lexeme::IsLess, "; break;
    case Lexer::Lexeme::IsGreater: std::cout << "{ Lexeme::IsGreater, "; break;
    case Lexer::Lexeme::IsLessOrEqual: std::cout << "{ Lexeme::IsLessOrEqual, "; break;
    case Lexer::Lexeme::IsGreaterOrEqual: std::cout << "{ Lexeme::IsGreaterOrEqual, "; break;
    case Lexer::Lexeme::Equal: std::cout << "{ Lexeme::Equal, "; break;
    case Lexer::Lexeme::Plus: std::cout << "{ Lexeme::Plus, "; break;
    case Lexer::Lexeme::Minus: std::cout << "{ Lexeme::Minus, "; break;
    case Lexer::Lexeme::Multiply: std::cout << "{ Lexeme::Multiply, "; break;
    case Lexer::Lexeme::Divide: std::cout << "{ Lexeme::Divide, "; break;
    case Lexer::Lexeme::Int: std::cout << "{ Lexeme::Int, "; break;
    case Lexer::Lexeme::Float: std::cout << "{ Lexeme::Float, "; break;
    case Lexer::Lexeme::String: std::cout << "{ Lexeme::String, "; break;

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

    for(auto i : ast.GetRootNode()->children)
        GetReturn(i, variables);
    
    for(auto [name, func] : functions)
        std::cout << name << " " << std::endl;
    for(auto [name, var] : variables)
        std::cout << name << " " << var->GetData().second << std::endl;
}

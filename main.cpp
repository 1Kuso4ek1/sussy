#include <AST.hpp>
#include <fstream>
#include <iostream>

void PrintAST(std::shared_ptr<AST::Node> node, int depth = 0)
{
    if(depth != 0) std::cout << "|";// << std::endl << "|";
    for(int i = 0; i < depth; i++) std::cout << "----";

    switch (node->expression.first)
    {
    case Lexer::Lexeme::None: std::cout << "Expression: { Lexeme::None, "; break;
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

    std::cout << ast.GetRootNode()->children[0]->GetReturn().second << std::endl;
}

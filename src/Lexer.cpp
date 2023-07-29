#include <Lexer.hpp>

Lexer::Lexer(const std::string& input)
{
    Lexer::Token res(Lexeme::None, "");
    bool opened_quote = false;
    auto add_and_clear = [&]()
    {
        if(res.first != Lexeme::None) tokens.push_back(res);
        res = { Lexeme::None, "" };
    };

    auto single_char = [&](Lexeme t, char& i)
    {
        if(!opened_quote)
        {
            add_and_clear();
            res = { t, std::string(1, i) };
            add_and_clear(); 
        } else res.second += i;
    };
    
    for(char i : input)
    {
        switch(i)
        {
        case '\"':
        case '\'':
            opened_quote = !opened_quote;
            res.first = Lexeme::String;
            break;

        case '{':
        case '}': single_char(i == '{' ? Lexeme::CurlyBraceOpen : Lexeme::CurlyBraceClose, i); break;
        case ';': single_char(Lexeme::Semicolon, i); break;
        case '=': single_char(Lexeme::Equal, i); break;
        case '+': single_char(Lexeme::Plus, i); break;
        case '-': single_char(Lexeme::Minus, i); break;
        case '*': single_char(Lexeme::Multiply, i); break;
        case '/': single_char(Lexeme::Divide, i); break;
        case '('...')': single_char(i == '(' ? Lexeme::BraceOpen : Lexeme::BraceClose, i); break;

        case 'A'...'z': 
            if(res.first == Lexeme::Word || opened_quote) res.second += i;
            else if(res.first == Lexeme::None) res = { Lexeme::Word, std::string(1, i) }; 
            break;
        
        case '0'...'9':
            if(res.first == Lexeme::Word || res.first == Lexeme::Int || res.first == Lexeme::Float || opened_quote) res.second += i;
            else if(res.first == Lexeme::None) res = { Lexeme::Int, std::string(1, i) }; 
            break;

        case '.':
            if(res.first == Lexeme::Int) { res.first = Lexeme::Float; res.second += i; }
            else if(opened_quote) res.second += i;
            else if(res.first == Lexeme::None) res = { Lexeme::Dot, std::string(1, i) }; 
            break;

        default:
            if(!opened_quote)
                add_and_clear();
            else res.second += i;
            break;
        }
    }
    add_and_clear();
}

std::vector<Lexer::Token> Lexer::Result()
{
    return tokens;
}

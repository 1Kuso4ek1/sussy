#include <Lexer.hpp>

Lexer::Lexer(const std::string& input)
{
    Lexer::Token res(Lexeme::None, "");
    bool openedQuote = false;
    auto addAndClear = [&]()
    {
        if(res.first == Lexeme::Word && std::find(reservedWords.begin(), reservedWords.end(), res.second) != reservedWords.end())
            res.first = Lexeme::ReservedWord;
        if(res.first != Lexeme::None) tokens.push_back(res);
        res = { Lexeme::None, "" };
    };

    auto singleChar = [&](Lexeme t, char i)
    {
        if(!openedQuote)
        {
            addAndClear();
            res = { t, std::string(1, i) };
            addAndClear(); 
        } else res.second += i;
    };
    
    for(char i : input)
    {
        switch(i)
        {
        case '\"':
        case '\'':
            openedQuote = !openedQuote;
            res.first = Lexeme::String;
            break;

        case '{': singleChar(Lexeme::CurlyBraceOpen, i); break;
        case '}': singleChar(Lexeme::CurlyBraceClose, i); break;
        case ',': singleChar(Lexeme::Comma, i); break;
        case ';': singleChar(Lexeme::Semicolon, i); break;
        case '=': singleChar(Lexeme::Equal, i); break;
        case '+': singleChar(Lexeme::Plus, i); break;
        case '-': singleChar(Lexeme::Minus, i); break;
        case '*': singleChar(Lexeme::Multiply, i); break;
        case '/': singleChar(Lexeme::Divide, i); break;
        case '('...')': singleChar(i == '(' ? Lexeme::BraceOpen : Lexeme::BraceClose, i); break;

        case 'A'...'z': 
            if(res.first == Lexeme::Word || openedQuote) res.second += i;
            else if(res.first == Lexeme::None) res = { Lexeme::Word, std::string(1, i) }; 
            break;
        
        case '0'...'9':
            if(res.first == Lexeme::Word || res.first == Lexeme::Int || res.first == Lexeme::Float || openedQuote) res.second += i;
            else if(res.first == Lexeme::None) res = { Lexeme::Int, std::string(1, i) }; 
            break;

        case '.':
            if(res.first == Lexeme::Int) { res.first = Lexeme::Float; res.second += i; }
            else if(openedQuote) res.second += i;
            else if(res.first == Lexeme::None) res = { Lexeme::Dot, std::string(1, i) }; 
            break;

        default:
            if(!openedQuote)
                addAndClear();
            else res.second += i;
            break;
        }
    }
    addAndClear();
}

std::vector<Lexer::Token> Lexer::Result()
{
    return tokens;
}

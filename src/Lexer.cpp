#include <Lexer.hpp>

Lexer::Lexer(const std::string& input)
{
    Lexer::Token res(Lexeme::None, "");
    bool openedQuote = false;
    bool openedComment = false;
    auto addAndClear = [&]()
    {
        if(openedComment)
        {
            res = { Lexeme::None, "" };
            return;
        }
        if(res.first == Lexeme::Word)
            if(std::find(reservedWords.begin(), reservedWords.end(), res.second) != reservedWords.end())
                res.first = Lexeme::ReservedWord;
            else if(res.second == "true" || res.second == "false")
                res.first = Lexeme::Bool;
        if(res.first != Lexeme::None) tokens.push_back(res);
        res = { Lexeme::None, "" };
    };

    auto singleChar = [&](Lexeme l, char i)
    {
        if(!openedQuote)
        {
            addAndClear();
            if(l == Lexeme::Equal && tokens.back().first == Lexeme::Equal)
            {
                tokens.back().first = Lexeme::IsEqual;
                tokens.back().second = "==";
                return;
            }
            else if(l == Lexeme::Equal && tokens.back().first == Lexeme::IsLess)
            {
                tokens.back().first = Lexeme::IsLessOrEqual;
                tokens.back().second = "<=";
                return;
            }
            else if(l == Lexeme::Equal && tokens.back().first == Lexeme::IsGreater)
            {
                tokens.back().first = Lexeme::IsGreaterOrEqual;
                tokens.back().second = ">=";
                return;
            }
            res = { l, std::string(1, i) };
            addAndClear();
        }
        else res.second += i;
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
        case '<': singleChar(Lexeme::IsLess, i); break;
        case '>': singleChar(Lexeme::IsGreater, i); break;
        case '('...')': singleChar(i == '(' ? Lexeme::BraceOpen : Lexeme::BraceClose, i); break;

        case '#': openedComment = !openedComment; break;

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

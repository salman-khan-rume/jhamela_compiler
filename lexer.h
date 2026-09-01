#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <unordered_map>
#include <vector>

enum TokenType {
    T_PURNOSONKHA, T_SHOTTOMULLO, T_SHOTTO, T_MITTHA, 
    T_NUMBER, T_STRING, T_IDENTIFIER,
    T_JODI, T_NAHOLE, T_JOTOKKHON, T_DEKHAO, T_PRODHAN, T_DHORO, T_RAKHO,
    T_PLUS, T_MINUS, T_MUL, T_DIV, T_ASSIGN_OP, T_GT,
    T_SEMICOLON, T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
    T_EOF, T_ERROR
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int rawIntValue; // Used to store pre-converted integer values for numbers
};

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();

private:
    std::string source;
    size_t pos;
    int line;
    std::unordered_map<std::string, TokenType> keywords;

    char peek();
    char advance();
    void skipWhitespace();
    std::string convertBengaliToEnglishDigits(std::string str);
    bool isStandardNumber(const std::string& str);
};
#endif
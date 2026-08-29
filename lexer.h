#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType {
    DHORO, RAKHO, PURNO_SONKHA, SOTTO_MULYO,
    JODI, NAHLE, JOKHON, DEKHAO, SOTTO, MITTHA,
    ID, NUMBER, STRING_LITERAL,
    PLUS, MINUS, MUL, DIV, ASSIGN,
    EQUAL_EQUAL, NOT_EQUAL, GREATER, LESS, GTE, LTE,
    AND, OR, NOT, LPAREN, RPAREN, LBRACE, RBRACE, SEMI,
    EOF_TOKEN, ERROR_TOKEN
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    int line;
    Token(TokenType type, std::string lexeme, int line);
    std::string toString() const;
};

class Lexer {
private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    std::unordered_map<std::string, TokenType> keywords;

    void scanToken();
    void identifier();
    void number();
    void stringLiteral();
    void skipWhitespaceAndComments();
    char advance();
    char peek();
    bool isAtEnd();
    void addToken(TokenType type);

public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();
};
#endif
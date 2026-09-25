#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <unordered_map>
#include <vector>

enum TokenType {
    // Type keywords
    T_PURNOSONKHA,   // পূর্ণসংখ্যা (int)
    T_SHOTTOMULLO,   // সত্যমূল্য (bool)
    T_TEXTO,         // টেক্সট      (string)

    // Boolean literals
    T_SHOTTO,        // সত্য  (true)
    T_MITTHA,        // মিথ্যা (false)

    // Values
    T_NUMBER, T_STRING, T_IDENTIFIER,

    // Statement keywords
    T_JODI, T_NAHOLE, T_JOTOKKHON, T_DEKHAO, T_DHORO, T_RAKHO,

    // Operators
    T_PLUS, T_MINUS, T_MUL, T_DIV, T_ASSIGN_OP,
    T_GT, T_LT, T_GE, T_LE, T_EQ, T_NEQ,

    // Punctuation
    T_SEMICOLON, T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,

    T_EOF
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int rawIntValue;   // pre-converted value for numbers
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
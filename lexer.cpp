#include "lexer.h"
#include <cctype>

using namespace std;

Token::Token(TokenType type, string lexeme, int line) : type(type), lexeme(lexeme), line(line) {}
string Token::toString() const { return lexeme; }

Lexer::Lexer(string source) : source(source) {
    keywords = {
        {"পূর্ণসংখ্যা", TokenType::PURNO_SONKHA}, {"সত্যমূল্য", TokenType::SOTTO_MULYO},
        {"যদি", TokenType::JODI}, {"নাহলে", TokenType::NAHLE},
        {"যতক্ষণ", TokenType::JOKHON}, {"দেখাও", TokenType::DEKHAO},
        {"সত্য", TokenType::SOTTO}, {"মিথ্যা", TokenType::MITTHA},
        {"ধরো", TokenType::DHORO}, {"রাখো", TokenType::RAKHO}
    };
}

vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", line));
    return tokens;
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') advance();
        else if (c == '\n') { line++; advance(); }
        else if (c == '/' && current + 1 < source.length() && source[current + 1] == '/') {
            while (peek() != '\n' && !isAtEnd()) advance();
        } else if (c == '/' && current + 1 < source.length() && source[current + 1] == '*') {
            advance(); advance();
            while (!isAtEnd()) {
                if (peek() == '\n') line++;
                if (peek() == '*' && current + 1 < source.length() && source[current + 1] == '/') {
                    advance(); advance();
                    break;
                }
                advance();
            }
        } else break;
    }
}

void Lexer::scanToken() {
    skipWhitespaceAndComments();
    if (isAtEnd()) return;
    start = current;
    unsigned char c = advance();

    switch (c) {
        case '(': addToken(TokenType::LPAREN); break;
        case ')': addToken(TokenType::RPAREN); break;
        case '{': addToken(TokenType::LBRACE); break;
        case '}': addToken(TokenType::RBRACE); break;
        case ';': addToken(TokenType::SEMI); break;
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(TokenType::MINUS); break;
        case '*': addToken(TokenType::MUL); break;
        case '/': addToken(TokenType::DIV); break;
        case '"': stringLiteral(); break;
        case '=': addToken(peek() == '=' ? (advance(), TokenType::EQUAL_EQUAL) : TokenType::ASSIGN); break;
        case '!': addToken(peek() == '=' ? (advance(), TokenType::NOT_EQUAL) : TokenType::NOT); break;
        case '>': addToken(peek() == '=' ? (advance(), TokenType::GTE) : TokenType::GREATER); break;
        case '<': addToken(peek() == '=' ? (advance(), TokenType::LTE) : TokenType::LESS); break;
        default:
            if (isdigit(c) || c >= 224) identifier();
            else if (isalpha(c)) identifier();
            else addToken(TokenType::ERROR_TOKEN);
            break;
    }
}

void Lexer::stringLiteral() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) { addToken(TokenType::ERROR_TOKEN); return; }
    advance();
    addToken(TokenType::STRING_LITERAL);
}

void Lexer::identifier() {
    while (isalnum(peek()) || (unsigned char)peek() >= 128) advance();
    string text = source.substr(start, current - start);
    if (keywords.count(text)) addToken(keywords[text]);
    else addToken(TokenType::ID);
}
char Lexer::advance() { return source[current++]; }
char Lexer::peek() { return isAtEnd() ? '\0' : source[current]; }
bool Lexer::isAtEnd() { return current >= source.length(); }
void Lexer::addToken(TokenType type) { tokens.push_back(Token(type, source.substr(start, current - start), line)); }
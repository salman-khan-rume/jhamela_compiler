#include "lexer.h"
#include <cctype>
<<<<<<< HEAD
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source), pos(0), line(1) {
    keywords["পূর্ণসংখ্যা"] = T_PURNOSONKHA;
    keywords["সত্যমূল্য"] = T_SHOTTOMULLO;
    keywords["সত্য"] = T_SHOTTO;
    keywords["মিথ্যা"] = T_MITTHA;
    keywords["যদি"] = T_JODI;
    keywords["নাহলে"] = T_NAHOLE;
    keywords["যতক্ষণ"] = T_JOTOKKHON;
    keywords["দেখাও"] = T_DEKHAO;
    keywords["প্রধান"] = T_PRODHAN;
    keywords["ধরো"] = T_DHORO;
    keywords["রাখো"] = T_RAKHO;
}

char Lexer::peek() { return pos < source.length() ? source[pos] : '\0'; }
char Lexer::advance() { return pos < source.length() ? source[pos++] : '\0'; }

void Lexer::skipWhitespace() {
    while (pos < source.length()) {
        char c = peek();
        if (isspace(c)) {
            if (c == '\n') line++;
            advance();
        } else { break; }
    }
}

std::string Lexer::convertBengaliToEnglishDigits(std::string str) {
    std::vector<std::pair<std::string, std::string>> b2e = {
        {"০", "0"}, {"১", "1"}, {"২", "2"}, {"৩", "3"}, {"৪", "4"},
        {"৫", "5"}, {"৬", "6"}, {"৭", "7"}, {"৮", "8"}, {"৯", "9"}
    };
    for (const auto& pair : b2e) {
        size_t start_pos = 0;
        while((start_pos = str.find(pair.first, start_pos)) != std::string::npos) {
            str.replace(start_pos, pair.first.length(), pair.second);
            start_pos += pair.second.length();
        }
    }
    return str;
}

bool Lexer::isStandardNumber(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) { if (!isdigit(c)) return false; }
    return true;
}

Token Lexer::getNextToken() {
    skipWhitespace();
    if (pos >= source.length()) return {T_EOF, "", line, 0};

    char c = peek();
    
    // Single-character delimiters and operators
    if (c == ';' || c == '=' || c == '>' || c == '+' || c == '-' || c == '*' || 
        c == '/' || c == '(' || c == ')' || c == '{' || c == '}') {
        advance();
        std::string val(1, c);
        switch (c) {
            case ';': return {T_SEMICOLON, val, line, 0};
            case '=': return {T_ASSIGN_OP, val, line, 0};
            case '>': return {T_GT, val, line, 0};
            case '+': return {T_PLUS, val, line, 0};
            case '-': return {T_MINUS, val, line, 0};
            case '*': return {T_MUL, val, line, 0};
            case '/': return {T_DIV, val, line, 0};
            case '{': return {T_LBRACE, val, line, 0};
            case '}': return {T_RBRACE, val, line, 0};
            case '(': return {T_LPAREN, val, line, 0};
            case ')': return {T_RPAREN, val, line, 0};
        }
    }

    // String literals
    if (c == '"') {
        advance();
        std::string str = "";
        while (peek() != '"' && peek() != '\0') {
            str += advance();
        }
        if (peek() == '"') advance();
        return {T_STRING, str, line, 0};
    }

    // Identifiers, Keywords, and Multi-byte Bengali Numbers
    std::string chunk = "";
    while (pos < source.length() && !isspace(peek()) && 
           peek() != ';' && peek() != '=' && peek() != '>' && 
           peek() != '+' && peek() != '-' && peek() != '*' && 
           peek() != '/' && peek() != '(' && peek() != ')' && 
           peek() != '{' && peek() != '}') {
        chunk += advance();
    }

    if (keywords.find(chunk) != keywords.end()) {
        return {keywords[chunk], chunk, line, 0};
    }

    std::string englishDigits = convertBengaliToEnglishDigits(chunk);
    if (isStandardNumber(englishDigits)) {
        return {T_NUMBER, chunk, line, std::stoi(englishDigits)};
    }

    return {T_IDENTIFIER, chunk, line, 0};
}
=======

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
>>>>>>> 33bc9ee728e088cf65328db8129d108aaeccf11d

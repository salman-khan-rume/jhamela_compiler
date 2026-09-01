#include "lexer.h"
#include <cctype>
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
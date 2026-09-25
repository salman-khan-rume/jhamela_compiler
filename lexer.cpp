#include "lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source), pos(0), line(1) {
    keywords["পূর্ণসংখ্যা"] = T_PURNOSONKHA;
    keywords["সত্যমূল্য"]   = T_SHOTTOMULLO;
    keywords["টেক্সট"]      = T_TEXTO;
    keywords["সত্য"]        = T_SHOTTO;
    keywords["মিথ্যা"]      = T_MITTHA;
    keywords["যদি"]         = T_JODI;
    keywords["নাহলে"]       = T_NAHOLE;
    keywords["যতক্ষণ"]      = T_JOTOKKHON;
    keywords["দেখাও"]       = T_DEKHAO;
    keywords["ধরো"]         = T_DHORO;
    keywords["রাখো"]        = T_RAKHO;
}

char Lexer::peek() { return pos < source.length() ? source[pos] : '\0'; }

char Lexer::advance() {
    return pos < source.length() ? source[pos++] : '\0';
}

void Lexer::skipWhitespace() {
    while (pos < source.length()) {
        unsigned char c = static_cast<unsigned char>(peek());
        if (std::isspace(c)) {
            if (c == '\n') line++;
            advance();
        } else {
            break;
        }
    }
}

std::string Lexer::convertBengaliToEnglishDigits(std::string str) {
    static const std::vector<std::pair<std::string, std::string>> b2e = {
        {"০","0"},{"১","1"},{"২","2"},{"৩","3"},{"৪","4"},
        {"৫","5"},{"৬","6"},{"৭","7"},{"৮","8"},{"৯","9"}
    };
    for (const auto& pair : b2e) {
        size_t p = 0;
        while ((p = str.find(pair.first, p)) != std::string::npos) {
            str.replace(p, pair.first.length(), pair.second);
            p += pair.second.length();
        }
    }
    return str;
}

bool Lexer::isStandardNumber(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

Token Lexer::getNextToken() {
    skipWhitespace();
    if (pos >= source.length()) return {T_EOF, "", line, 0};

    char c    = peek();
    char next = (pos + 1 < source.length()) ? source[pos + 1] : '\0';

    // ---- Two-character operators ----
    if (c == '>' && next == '=') { advance(); advance(); return {T_GE,  ">=", line, 0}; }
    if (c == '<' && next == '=') { advance(); advance(); return {T_LE,  "<=", line, 0}; }
    if (c == '=' && next == '=') { advance(); advance(); return {T_EQ,  "==", line, 0}; }
    if (c == '!' && next == '=') { advance(); advance(); return {T_NEQ, "!=", line, 0}; }

    // ---- Single-character operators / punctuation ----
    if (c == ';' || c == '=' || c == '>' || c == '<' || c == '+' || c == '-' ||
        c == '*' || c == '/' || c == '(' || c == ')' || c == '{' || c == '}') {
        advance();
        std::string val(1, c);
        switch (c) {
            case ';': return {T_SEMICOLON, val, line, 0};
            case '=': return {T_ASSIGN_OP, val, line, 0};
            case '>': return {T_GT,        val, line, 0};
            case '<': return {T_LT,        val, line, 0};
            case '+': return {T_PLUS,      val, line, 0};
            case '-': return {T_MINUS,     val, line, 0};
            case '*': return {T_MUL,       val, line, 0};
            case '/': return {T_DIV,       val, line, 0};
            case '{': return {T_LBRACE,    val, line, 0};
            case '}': return {T_RBRACE,    val, line, 0};
            case '(': return {T_LPAREN,    val, line, 0};
            case ')': return {T_RPAREN,    val, line, 0};
        }
    }

    // ---- String literal ----
    if (c == '"') {
        advance();
        std::string str;
        while (peek() != '"' && peek() != '\0') {
            str += advance();
        }
        if (peek() == '"') advance();
        return {T_STRING, str, line, 0};
    }

    // ---- Identifier / keyword / Bengali number ----
    std::string chunk;
    while (pos < source.length()) {
        char x = peek();
        if (std::isspace(static_cast<unsigned char>(x)) ||
            x == ';' || x == '=' || x == '>' || x == '<' || x == '!' ||
            x == '+' || x == '-' || x == '*' || x == '/' ||
            x == '(' || x == ')' || x == '{' || x == '}' || x == '"') {
            break;
        }
        chunk += advance();
    }

    auto it = keywords.find(chunk);
    if (it != keywords.end()) return {it->second, chunk, line, 0};

    std::string englishDigits = convertBengaliToEnglishDigits(chunk);
    if (isStandardNumber(englishDigits)) {
        return {T_NUMBER, chunk, line, std::stoi(englishDigits)};
    }

    return {T_IDENTIFIER, chunk, line, 0};
}
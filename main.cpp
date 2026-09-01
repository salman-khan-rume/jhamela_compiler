#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.h"
#include "parser.h"

std::string formatToken(const Token& t) {
    switch (t.type) {
        case T_DHORO: case T_PURNOSONKHA: case T_SHOTTOMULLO: case T_JODI: 
        case T_NAHOLE: case T_JOTOKKHON: case T_DEKHAO: case T_PRODHAN: case T_RAKHO:
            return "KEYWORD(" + t.value + ")";
        case T_IDENTIFIER: return "IDENTIFIER(" + t.value + ")";
        case T_NUMBER: return "NUMBER(" + std::to_string(t.rawIntValue) + ")";
        case T_STRING: return "STRING(\"" + t.value + "\")";
        case T_ASSIGN_OP: return "ASSIGN(" + t.value + ")";
        case T_PLUS: case T_MINUS: case T_MUL: case T_DIV: case T_GT:
            return "OPERATOR(" + t.value + ")";
        case T_SEMICOLON: case T_LBRACE: case T_RBRACE: case T_LPAREN: case T_RPAREN:
            return "PUNCTUATION(" + t.value + ")";
        default: return "UNKNOWN(" + t.value + ")";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.jh>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    std::vector<Token> tokens;
    
    // 1. Lexical Analysis
    Token t = lexer.getNextToken();
    while (t.type != T_EOF && t.type != T_ERROR) {
        tokens.push_back(t);
        t = lexer.getNextToken();
    }
    
    // 2. Token Console Output
    std::cout << "=== TOKENS ===" << std::endl;
    std::cout << "Total Tokens: " << tokens.size() << std::endl;
    for (const auto& tok : tokens) {
        std::cout << formatToken(tok) << std::endl;
    }
    
    // 3. Parsing & Tree Generation
    std::cout << "\n=== PARSE TREE ===" << std::endl;
    Parser parser(tokens);
    auto ast = parser.parseProgram();
    ast->print();

    return 0;
}
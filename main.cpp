#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"

std::string formatToken(const Token& t) {
    switch (t.type) {
        case T_DHORO: case T_RAKHO:
        case T_PURNOSONKHA: case T_SHOTTOMULLO: case T_TEXTO:
        case T_SHOTTO: case T_MITTHA:
        case T_JODI: case T_NAHOLE: case T_JOTOKKHON: case T_DEKHAO:
            return "KEYWORD(" + t.value + ")";
        case T_IDENTIFIER: return "IDENTIFIER(" + t.value + ")";
        case T_NUMBER:     return "NUMBER(" + std::to_string(t.rawIntValue) + ")";
        case T_STRING:     return "STRING(\"" + t.value + "\")";
        case T_ASSIGN_OP:  return "ASSIGN(" + t.value + ")";
        case T_PLUS: case T_MINUS: case T_MUL: case T_DIV:
        case T_GT: case T_LT: case T_GE: case T_LE: case T_EQ: case T_NEQ:
            return "OPERATOR(" + t.value + ")";
        case T_SEMICOLON: case T_LBRACE: case T_RBRACE:
        case T_LPAREN: case T_RPAREN:
            return "PUNCTUATION(" + t.value + ")";
        case T_EOF: return "EOF";
    }
    return "UNKNOWN(" + t.value + ")";
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

    try {
        // ---- LEXICAL ANALYSIS ----
        Lexer lexer(source);
        std::vector<Token> tokens;
        Token t = lexer.getNextToken();
        while (t.type != T_EOF) {
            tokens.push_back(t);
            t = lexer.getNextToken();
        }

        std::cout << "=== TOKENS ===" << std::endl;
        std::cout << "Total Tokens: " << tokens.size() << std::endl;
        for (const auto& tok : tokens)
            std::cout << formatToken(tok) << std::endl;

        // ---- PARSING & AST ----
        std::cout << "\n=== PARSE TREE ===" << std::endl;
        Parser parser(tokens);
        auto ast = parser.parseProgram();
        ast->print();

        // ---- SEMANTIC ANALYSIS ----
        std::cout << "\n=== SEMANTIC ANALYSIS ===" << std::endl;
        SemanticAnalyzer semanticAnalyzer;
        semanticAnalyzer.analyze(ast.get());
        std::cout << "✓ Semantic analysis passed" << std::endl;
        semanticAnalyzer.getSymbolTable().printSymbols();

        // ---- CODE GENERATION ----
        std::cout << "\n=== CODE GENERATION ===" << std::endl;
        CodeGenerator codegen;
        std::string pythonCode = codegen.generate(ast.get());

        std::ofstream outFile("output.py");
        outFile << pythonCode;
        outFile.close();

        std::cout << "✓ Generated Python code (output.py):\n" << std::endl;
        std::cout << pythonCode << std::endl;

        std::cout << "\n=== COMPILATION SUCCESSFUL ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n✗ COMPILATION ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
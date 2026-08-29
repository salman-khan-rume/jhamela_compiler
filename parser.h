#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <stdexcept>
#include "lexer.h"
#include "ast.h"

class Parser {
private:
    std::vector<Token> tokens;
    int current = 0;

    ASTNode* statement();
    ASTNode* declaration();
    ASTNode* printStatement();
    ASTNode* ifStatement();
    ASTNode* whileStatement();
    ASTNode* expression();
    ASTNode* equality();
    ASTNode* comparison();
    ASTNode* term();
    ASTNode* factor();
    ASTNode* unary();
    ASTNode* primary();

    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, std::string message);
    void synchronize();

public:
    Parser(std::vector<Token> tokens);
    std::vector<ASTNode*> parse();
};
#endif
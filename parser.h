#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "ast.h"
#include <vector>
#include <stdexcept>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<BlockNode> parseProgram();

private:
    std::vector<Token> tokens;
    size_t pos;
    Token currentToken;

    void advance();
    void match(TokenType type);
    void synchronize(); 

    std::unique_ptr<StmtNode> parseStatement();
    std::unique_ptr<BlockNode> parseBlock();
    std::unique_ptr<ExprNode> parseExpression();
    std::unique_ptr<ExprNode> parseComparison();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();
};
#endif
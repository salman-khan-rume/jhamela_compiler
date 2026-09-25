#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "ast.h"
#include <vector>
#include <stdexcept>
#include <memory>

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

    // Statements
    std::unique_ptr<StmtNode>  parseStatement();
    std::unique_ptr<BlockNode> parseBlock();

    // Expressions (precedence low -> high)
    std::unique_ptr<ExprNode> parseExpression();   // comparison
    std::unique_ptr<ExprNode> parseComparison();
    std::unique_ptr<ExprNode> parseAdditive();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();       // unary
    std::unique_ptr<ExprNode> parsePrimary();      // literal / ident / ( )
};
#endif
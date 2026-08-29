#include "parser.h"
#include <iostream>

using namespace std;

Parser::Parser(vector<Token> tokens) : tokens(tokens) {}

vector<ASTNode*> Parser::parse() {
    vector<ASTNode*> statements;
    while (!isAtEnd()) {
        try { statements.push_back(statement()); }
        catch (const runtime_error& error) {
            cerr << error.what() << "\n";
            synchronize(); 
        }
    }
    return statements;
}

ASTNode* Parser::statement() {
    if (match({TokenType::DHORO})) return declaration();
    if (match({TokenType::RAKHO})) {
        Token name = consume(TokenType::ID, "Expected variable name after রাখো.");
        consume(TokenType::ASSIGN, "Expected '='.");
        ASTNode* expr = expression();
        consume(TokenType::SEMI, "Expected ';'.");
        return new AssignNode(VarType::INT, name.lexeme, expr); // Type logic handled in semantic phase
    }
    if (match({TokenType::DEKHAO})) return printStatement();
    if (match({TokenType::JODI})) return ifStatement();
    if (match({TokenType::JOKHON})) return whileStatement();

    ASTNode* expr = expression();
    consume(TokenType::SEMI, "Expected ';' after expression.");
    return expr;
}

ASTNode* Parser::declaration() {
    VarType type = match({TokenType::PURNO_SONKHA}) ? VarType::INT : VarType::BOOL;
    if (previous().type != TokenType::PURNO_SONKHA) consume(TokenType::SOTTO_MULYO, "Expected data type.");
    
    Token name = consume(TokenType::ID, "Expected variable name.");
    ASTNode* expr = nullptr;
    if (match({TokenType::ASSIGN})) expr = expression();
    consume(TokenType::SEMI, "Expected ';' after declaration.");
    return new AssignNode(type, name.lexeme, expr ? expr : new NumberNode(0));
}

ASTNode* Parser::printStatement() {
    ASTNode* expr = expression();
    consume(TokenType::SEMI, "Expected ';'.");
    return new PrintNode(expr);
}

ASTNode* Parser::ifStatement() {
    ASTNode* condition = expression();
    consume(TokenType::LBRACE, "Expected '{'.");
    vector<ASTNode*> thenBody;
    while (!check(TokenType::RBRACE) && !isAtEnd()) thenBody.push_back(statement());
    consume(TokenType::RBRACE, "Expected '}'.");
    
    vector<ASTNode*> elseBody;
    if (match({TokenType::NAHLE})) {
        consume(TokenType::LBRACE, "Expected '{'.");
        while (!check(TokenType::RBRACE) && !isAtEnd()) elseBody.push_back(statement());
        consume(TokenType::RBRACE, "Expected '}'.");
    }
    return new IfNode(condition, thenBody, elseBody);
}

ASTNode* Parser::whileStatement() {
    ASTNode* condition = expression();
    consume(TokenType::LBRACE, "Expected '{'.");
    vector<ASTNode*> body;
    while (!check(TokenType::RBRACE) && !isAtEnd()) body.push_back(statement());
    consume(TokenType::RBRACE, "Expected '}'.");
    return new WhileNode(condition, body);
}

ASTNode* Parser::expression() { return equality(); }
ASTNode* Parser::equality() {
    ASTNode* expr = comparison();
    while (match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
        Token op = previous();
        expr = new BinOpNode(expr, op.lexeme, comparison());
    }
    return expr;
}
ASTNode* Parser::comparison() {
    ASTNode* expr = term();
    while (match({TokenType::GREATER, TokenType::LESS, TokenType::GTE, TokenType::LTE})) {
        Token op = previous();
        expr = new BinOpNode(expr, op.lexeme, term());
    }
    return expr;
}
ASTNode* Parser::term() {
    ASTNode* expr = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        expr = new BinOpNode(expr, op.lexeme, factor());
    }
    return expr;
}
ASTNode* Parser::factor() {
    ASTNode* expr = unary();
    while (match({TokenType::MUL, TokenType::DIV})) {
        Token op = previous();
        expr = new BinOpNode(expr, op.lexeme, unary());
    }
    return expr;
}
ASTNode* Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) return new UnaryOpNode(previous().lexeme, unary());
    return primary();
}
ASTNode* Parser::primary() {
    if (match({TokenType::SOTTO})) return new BoolNode(true);
    if (match({TokenType::MITTHA})) return new BoolNode(false);
    if (match({TokenType::NUMBER})) return new NumberNode(stoi(previous().lexeme));
    if (match({TokenType::STRING_LITERAL})) return new StringNode(previous().lexeme);
    if (match({TokenType::ID})) return new VarNode(previous().lexeme);
    if (match({TokenType::LPAREN})) {
        ASTNode* expr = expression();
        consume(TokenType::RPAREN, "Expected ')'.");
        return expr;
    }
    throw runtime_error("Error at line " + to_string(peek().line) + ": Expected expression.");
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) if (check(type)) { advance(); return true; }
    return false;
}
bool Parser::check(TokenType type) { return !isAtEnd() && peek().type == type; }
Token Parser::advance() { if (!isAtEnd()) current++; return previous(); }
bool Parser::isAtEnd() { return peek().type == TokenType::EOF_TOKEN; }
Token Parser::peek() { return tokens[current]; }
Token Parser::previous() { return tokens[current - 1]; }
Token Parser::consume(TokenType type, string message) {
    if (check(type)) return advance();
    throw runtime_error("Error at line " + to_string(peek().line) + ": " + message);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMI) return;
        switch (peek().type) {
            case TokenType::DHORO: case TokenType::RAKHO: case TokenType::JODI: 
            case TokenType::JOKHON: case TokenType::DEKHAO: return;
            default: break;
        }
        advance();
    }
}
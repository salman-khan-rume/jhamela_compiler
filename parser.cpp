#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {
    if (!this->tokens.empty()) currentToken = this->tokens[pos];
}

void Parser::advance() {
    pos++;
    if (pos < tokens.size()) currentToken = tokens[pos];
    else currentToken = {T_EOF, "", -1, 0};
}

void Parser::match(TokenType type) {
    if (currentToken.type == type) {
        advance();
    } else {
        throw std::runtime_error("Syntax error: unexpected token '" + currentToken.value + "'");
    }
}

// Satisfies mandatory feature: basic syntax error recovery
void Parser::synchronize() {
    while (currentToken.type != T_EOF) {
        if (currentToken.type == T_SEMICOLON) {
            advance();
            return;
        }
        advance();
    }
}

std::unique_ptr<BlockNode> Parser::parseProgram() {
    auto program = std::make_unique<BlockNode>();
    while (currentToken.type != T_EOF) {
        try {
            program->statements.push_back(parseStatement());
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            synchronize(); 
        }
    }
    return program;
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    match(T_LBRACE);
    auto block = std::make_unique<BlockNode>();
    while (currentToken.type != T_RBRACE && currentToken.type != T_EOF) {
        try {
            block->statements.push_back(parseStatement());
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            synchronize(); 
        }
    }
    match(T_RBRACE);
    return block;
}

// FIX: Accept all type keywords (int, bool, string)
bool isTypeKeyword(TokenType type) {
    return type == T_PURNOSONKHA || 
           type == T_SHOTTOMULLO || 
           type == T_STRING;  
}

std::unique_ptr<StmtNode> Parser::parseStatement() {
    // Variable declaration: ধরো পূর্ণসংখ্যা x;
    if (currentToken.type == T_DHORO) { 
        auto node = std::make_unique<VarDeclNode>();
        match(T_DHORO);
        
        // FIX: Accept any type keyword instead of hardcoding T_PURNOSONKHA
        if (!isTypeKeyword(currentToken.type)) {
            throw std::runtime_error("Expected type keyword after 'ধরো', got '" + currentToken.value + "'");
        }
        
        node->type = currentToken.value;  // Store type string
        advance();  // Accept whichever type keyword
        
        node->name = currentToken.value;
        match(T_IDENTIFIER);
        match(T_SEMICOLON);
        return node;
    } 
    // Assignment: রাখো x = expr;
    else if (currentToken.type == T_RAKHO) { 
        match(T_RAKHO);
        auto node = std::make_unique<AssignNode>();
        node->name = currentToken.value;
        match(T_IDENTIFIER);
        match(T_ASSIGN_OP); 
        node->expr = parseExpression();
        match(T_SEMICOLON);
        return node;
    }
    // If statement: যদি (expr) { ... }
    else if (currentToken.type == T_JODI) { 
        auto node = std::make_unique<IfNode>();
        match(T_JODI);
        node->condition = parseExpression();
        node->thenBranch = parseBlock();
        if (currentToken.type == T_NAHOLE) {
            match(T_NAHOLE);
            node->elseBranch = parseBlock();
        }
        return node;
    }
    // While loop: যতক্ষণ (expr) { ... }
    else if (currentToken.type == T_JOTOKKHON) { 
        auto node = std::make_unique<WhileNode>();
        match(T_JOTOKKHON);
        node->condition = parseExpression();
        node->body = parseBlock();
        return node;
    }
    // Print statement: দেখাও expr;
    else if (currentToken.type == T_DEKHAO) { 
        auto node = std::make_unique<PrintNode>();
        match(T_DEKHAO);
        node->expr = parseExpression();
        match(T_SEMICOLON);
        return node;
    }
    throw std::runtime_error("Unknown statement starting with '" + currentToken.value + "'");
}

std::unique_ptr<ExprNode> Parser::parseExpression() {
    auto left = parseComparison();
    while (currentToken.type == T_PLUS || currentToken.type == T_MINUS) {
        std::string op = currentToken.value;
        advance();
        auto right = parseComparison();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<ExprNode> Parser::parseComparison() {
    auto left = parseTerm();
    while (currentToken.type == T_GT) {
        std::string op = currentToken.value;
        advance();
        auto right = parseTerm();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<ExprNode> Parser::parseTerm() {
    auto left = parseFactor();
    while (currentToken.type == T_MUL || currentToken.type == T_DIV) {
        std::string op = currentToken.value;
        advance();
        auto right = parseFactor();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<ExprNode> Parser::parseFactor() {
    if (currentToken.type == T_NUMBER) {
        auto node = std::make_unique<NumberNode>(currentToken.rawIntValue);
        match(T_NUMBER);
        return node;
    } else if (currentToken.type == T_STRING) {
        auto node = std::make_unique<StringNode>(currentToken.value);
        match(T_STRING);
        return node;
    } else if (currentToken.type == T_IDENTIFIER) {
        auto node = std::make_unique<IdentifierNode>(currentToken.value);
        match(T_IDENTIFIER);
        return node;
    } else if (currentToken.type == T_LPAREN) {
        match(T_LPAREN);
        auto node = parseExpression();
        match(T_RPAREN);
        return node;
    }
    throw std::runtime_error("Expected expression factor, got '" + currentToken.value + "'");
}
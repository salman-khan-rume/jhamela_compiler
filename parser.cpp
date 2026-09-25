#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {
    if (!this->tokens.empty()) currentToken = this->tokens[pos];
    else currentToken = {T_EOF, "", 0, 0};
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
        throw std::runtime_error("Syntax error at line " +
                                 std::to_string(currentToken.line) +
                                 ": unexpected token '" + currentToken.value + "'");
    }
}

// Basic syntax error recovery:
//  - stop after consuming ';'
//  - stop before '}' (let parseBlock handle it)
//  - stop when line number advances (end of line)
//  - always consume at least one token so we never loop forever
void Parser::synchronize() {
    if (currentToken.type == T_EOF) return;

    int errorLine = currentToken.line;
    advance();   // guarantee progress

    while (currentToken.type != T_EOF) {
        if (currentToken.type == T_SEMICOLON) { advance(); return; }
        if (currentToken.type == T_RBRACE)    return;
        if (currentToken.line > errorLine)    return;
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

// Accept all three real type keywords
static bool isTypeKeyword(TokenType type) {
    return type == T_PURNOSONKHA ||
           type == T_SHOTTOMULLO ||
           type == T_TEXTO;
}

std::unique_ptr<StmtNode> Parser::parseStatement() {
    // ধরো <type> <ident> ;
    if (currentToken.type == T_DHORO) {
        auto node = std::make_unique<VarDeclNode>();
        match(T_DHORO);
        if (!isTypeKeyword(currentToken.type)) {
            throw std::runtime_error("Expected type keyword after 'ধরো', got '" +
                                     currentToken.value + "'");
        }
        node->type = currentToken.value;
        advance();
        node->name = currentToken.value;
        match(T_IDENTIFIER);
        match(T_SEMICOLON);
        return node;
    }
    // রাখো <ident> = <expr> ;
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
    // যদি <expr> { } [নাহলে { }]
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
    // যতক্ষণ <expr> { }
    else if (currentToken.type == T_JOTOKKHON) {
        auto node = std::make_unique<WhileNode>();
        match(T_JOTOKKHON);
        node->condition = parseExpression();
        node->body = parseBlock();
        return node;
    }
    // দেখাও <expr> ;
    else if (currentToken.type == T_DEKHAO) {
        auto node = std::make_unique<PrintNode>();
        match(T_DEKHAO);
        node->expr = parseExpression();
        match(T_SEMICOLON);
        return node;
    }
    throw std::runtime_error("Unknown statement starting with '" +
                             currentToken.value + "'");
}

// Precedence (lowest -> highest):
//   comparison  > < >= <= == !=
//   additive    + -
//   term        * /
//   unary       -
//   primary     number | bool | string | ident | ( expr )

std::unique_ptr<ExprNode> Parser::parseExpression() {
    return parseComparison();
}

std::unique_ptr<ExprNode> Parser::parseComparison() {
    auto left = parseAdditive();
    while (currentToken.type == T_GT || currentToken.type == T_LT ||
           currentToken.type == T_GE || currentToken.type == T_LE ||
           currentToken.type == T_EQ || currentToken.type == T_NEQ) {
        std::string op = currentToken.value;
        advance();
        auto right = parseAdditive();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<ExprNode> Parser::parseAdditive() {
    auto left = parseTerm();
    while (currentToken.type == T_PLUS || currentToken.type == T_MINUS) {
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
    if (currentToken.type == T_MINUS) {
        advance();
        auto operand = parseFactor();          // allow --x, -(a+b), etc.
        return std::make_unique<UnaryOpNode>("-", std::move(operand));
    }
    return parsePrimary();
}

std::unique_ptr<ExprNode> Parser::parsePrimary() {
    if (currentToken.type == T_NUMBER) {
        auto n = std::make_unique<NumberNode>(currentToken.rawIntValue);
        match(T_NUMBER);
        return n;
    }
    if (currentToken.type == T_STRING) {
        auto n = std::make_unique<StringNode>(currentToken.value);
        match(T_STRING);
        return n;
    }
    if (currentToken.type == T_SHOTTO) {
        auto n = std::make_unique<BoolNode>(true);
        match(T_SHOTTO);
        return n;
    }
    if (currentToken.type == T_MITTHA) {
        auto n = std::make_unique<BoolNode>(false);
        match(T_MITTHA);
        return n;
    }
    if (currentToken.type == T_IDENTIFIER) {
        auto n = std::make_unique<IdentifierNode>(currentToken.value);
        match(T_IDENTIFIER);
        return n;
    }
    if (currentToken.type == T_LPAREN) {
        match(T_LPAREN);
        auto n = parseExpression();
        match(T_RPAREN);
        return n;
    }
    throw std::runtime_error("Expected expression, got '" + currentToken.value + "'");
}
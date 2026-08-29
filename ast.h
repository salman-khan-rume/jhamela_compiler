#ifndef AST_H
#define AST_H
#include <string>
#include <vector>
#include <iostream>

enum class VarType { INT, BOOL, STRING };

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

inline void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}

class NumberNode : public ASTNode {
public:
    int value;
    NumberNode(int value) : value(value) {}
    void print(int indent = 0) const override { printIndent(indent); std::cout << "Number: " << value << "\n"; }
};

class BoolNode : public ASTNode {
public:
    bool value;
    BoolNode(bool value) : value(value) {}
    void print(int indent = 0) const override { printIndent(indent); std::cout << "Bool: " << (value ? "সত্য" : "মিথ্যা") << "\n"; }
};

class StringNode : public ASTNode {
public:
    std::string value;
    StringNode(std::string value) : value(value) {}
    void print(int indent = 0) const override { printIndent(indent); std::cout << "String: " << value << "\n"; }
};

class VarNode : public ASTNode {
public:
    std::string name;
    VarNode(std::string name) : name(name) {}
    void print(int indent = 0) const override { printIndent(indent); std::cout << "Var: " << name << "\n"; }
};

class BinOpNode : public ASTNode {
public:
    ASTNode* left;
    std::string op;
    ASTNode* right;
    BinOpNode(ASTNode* left, std::string op, ASTNode* right) : left(left), op(op), right(right) {}
    void print(int indent = 0) const override {
        printIndent(indent); std::cout << "BinOp (" << op << ")\n";
        left->print(indent + 1);
        right->print(indent + 1);
    }
};

class UnaryOpNode : public ASTNode {
public:
    std::string op;
    ASTNode* expr;
    UnaryOpNode(std::string op, ASTNode* expr) : op(op), expr(expr) {}
    void print(int indent = 0) const override {
        printIndent(indent); std::cout << "UnaryOp (" << op << ")\n";
        expr->print(indent + 1);
    }
};

class AssignNode : public ASTNode {
public:
    VarType type;
    std::string name;
    ASTNode* expr;
    AssignNode(VarType type, std::string name, ASTNode* expr) : type(type), name(name), expr(expr) {}
    void print(int indent = 0) const override {
        printIndent(indent); std::cout << "Assign (" << name << ")\n";
        expr->print(indent + 1);
    }
};

class PrintNode : public ASTNode {
public:
    ASTNode* expr;
    PrintNode(ASTNode* expr) : expr(expr) {}
    void print(int indent = 0) const override {
        printIndent(indent); std::cout << "Print\n";
        expr->print(indent + 1);
    }
};

class IfNode : public ASTNode {
public:
    ASTNode* condition;
    std::vector<ASTNode*> thenBody;
    std::vector<ASTNode*> elseBody;
    IfNode(ASTNode* condition, std::vector<ASTNode*> thenBody, std::vector<ASTNode*> elseBody = {})
        : condition(condition), thenBody(thenBody), elseBody(elseBody) {}
    void print(int indent = 0) const override {
        printIndent(indent); std::cout << "If\n";
        condition->print(indent + 1);
        printIndent(indent); std::cout << "Then:\n";
        for (auto stmt : thenBody) stmt->print(indent + 1);
        if (!elseBody.empty()) {
            printIndent(indent); std::cout << "Else:\n";
            for (auto stmt : elseBody) stmt->print(indent + 1);
        }
    }
};

class WhileNode : public ASTNode {
public:
    ASTNode* condition;
    std::vector<ASTNode*> body;
    WhileNode(ASTNode* condition, std::vector<ASTNode*> body) : condition(condition), body(body) {}
    void print(int indent = 0) const override {
        printIndent(indent); std::cout << "While\n";
        condition->print(indent + 1);
        printIndent(indent); std::cout << "Body:\n";
        for (auto stmt : body) stmt->print(indent + 1);
    }
};
#endif
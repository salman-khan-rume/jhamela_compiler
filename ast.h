#ifndef AST_H
#define AST_H
#include <string>
#include <vector>
#include <memory>
#include <iostream>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(std::string prefix = "", bool isLast = true) const = 0;
};

class ExprNode : public ASTNode {};
class StmtNode : public ASTNode {};

class BlockNode : public StmtNode {
public:
    std::vector<std::unique_ptr<StmtNode>> statements;
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "Block\n";
        prefix += (isLast ? "    " : "│   ");
        for (size_t i = 0; i < statements.size(); ++i) {
            statements[i]->print(prefix, i == statements.size() - 1);
        }
    }
};

class NumberNode : public ExprNode {
public:
    int value;
    NumberNode(int v) : value(v) {}
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "Number: " << value << "\n";
    }
};

class StringNode : public ExprNode {
public:
    std::string value;
    StringNode(std::string v) : value(v) {}
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "String: \"" << value << "\"\n";
    }
};

class IdentifierNode : public ExprNode {
public:
    std::string name;
    IdentifierNode(std::string n) : name(n) {}
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "Var: " << name << "\n";
    }
};

class BinaryOpNode : public ExprNode {
public:
    std::unique_ptr<ExprNode> left;
    std::string op;
    std::unique_ptr<ExprNode> right;
    BinaryOpNode(std::unique_ptr<ExprNode> l, std::string o, std::unique_ptr<ExprNode> r) 
        : left(std::move(l)), op(o), right(std::move(r)) {}
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "BinOp (" << op << ")\n";
        prefix += (isLast ? "    " : "│   ");
        left->print(prefix, false);
        right->print(prefix, true);
    }
};

class VarDeclNode : public StmtNode {
public:
    std::string type;
    std::string name;
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "Declaration\n";
        prefix += (isLast ? "    " : "│   ");
        std::cout << prefix << "├── Keyword: " << type << "\n";
        std::cout << prefix << "└── Identifier: " << name << "\n";
    }
};

class AssignNode : public StmtNode {
public:
    std::string name;
    std::unique_ptr<ExprNode> expr;
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "Assign (" << name << ")\n";
        prefix += (isLast ? "    " : "│   ");
        expr->print(prefix, true);
    }
};

class PrintNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> expr;
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "Print\n";
        prefix += (isLast ? "    " : "│   ");
        expr->print(prefix, true);
    }
};

class IfNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<BlockNode> thenBranch;
    std::unique_ptr<BlockNode> elseBranch;
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "If\n";
        std::string newPrefix = prefix + (isLast ? "    " : "│   ");
        condition->print(newPrefix, false);
        std::cout << newPrefix << "├── Then:\n";
        thenBranch->print(newPrefix + "│   ", elseBranch == nullptr);
        if (elseBranch) {
            std::cout << newPrefix << "└── Else:\n";
            elseBranch->print(newPrefix + "    ", true);
        }
    }
};

class WhileNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<BlockNode> body;
    void print(std::string prefix = "", bool isLast = true) const override {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "While\n";
        std::string newPrefix = prefix + (isLast ? "    " : "│   ");
        condition->print(newPrefix, false);
        std::cout << newPrefix << "└── Body:\n";
        body->print(newPrefix + "    ", true);
    }
};
#endif
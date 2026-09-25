#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "ast.h"
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>

enum class VarType { INT, STRING, BOOL, UNDEFINED };

struct Symbol {
    std::string name;
    VarType type;
    bool initialized;
    Symbol() : name(""), type(VarType::UNDEFINED), initialized(false) {}
    Symbol(std::string n, VarType t, bool init = false)
        : name(n), type(t), initialized(init) {}
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> symbols;

public:
    void define(const std::string& name, VarType type) {
        if (symbols.find(name) != symbols.end())
            throw std::runtime_error("Variable '" + name + "' already declared");
        symbols[name] = Symbol(name, type, false);
    }

    void initialize(const std::string& name) {
        auto it = symbols.find(name);
        if (it == symbols.end())
            throw std::runtime_error("Variable '" + name + "' not declared");
        it->second.initialized = true;
    }

    Symbol* lookup(const std::string& name) {
        auto it = symbols.find(name);
        return it == symbols.end() ? nullptr : &it->second;
    }

    bool exists(const std::string& name) {
        return symbols.find(name) != symbols.end();
    }

    void printSymbols() {
        std::cout << "\n=== SYMBOL TABLE ===" << std::endl;
        for (const auto& pair : symbols) {
            const Symbol& sym = pair.second;
            std::string typeStr;
            switch (sym.type) {
                case VarType::INT:       typeStr = "int";       break;
                case VarType::STRING:    typeStr = "string";    break;
                case VarType::BOOL:      typeStr = "bool";      break;
                case VarType::UNDEFINED: typeStr = "undefined"; break;
            }
            std::cout << sym.name << " : " << typeStr
                      << " (" << (sym.initialized ? "yes" : "no") << ")"
                      << std::endl;
        }
    }
};

class SemanticAnalyzer {
private:
    SymbolTable symTable;

public:
    SemanticAnalyzer() = default;

    void analyze(BlockNode* program) {
        for (auto& stmt : program->statements)
            analyzeStatement(stmt.get());
    }

    SymbolTable& getSymbolTable() { return symTable; }

private:
    void analyzeStatement(StmtNode* stmt) {
        if (auto s = dynamic_cast<VarDeclNode*>(stmt))      analyzeVarDecl(s);
        else if (auto s = dynamic_cast<AssignNode*>(stmt))  analyzeAssign(s);
        else if (auto s = dynamic_cast<PrintNode*>(stmt))   analyzePrint(s);
        else if (auto s = dynamic_cast<IfNode*>(stmt))      analyzeIf(s);
        else if (auto s = dynamic_cast<WhileNode*>(stmt))   analyzeWhile(s);
        else if (auto s = dynamic_cast<BlockNode*>(stmt))   analyzeBlock(s);
    }

    void analyzeVarDecl(VarDeclNode* decl) {
        VarType type = VarType::UNDEFINED;
        if      (decl->type == "পূর্ণসংখ্যা") type = VarType::INT;
        else if (decl->type == "সত্যমূল্য")   type = VarType::BOOL;
        else if (decl->type == "টেক্সট")      type = VarType::STRING;

        if (type == VarType::UNDEFINED)
            throw std::runtime_error("Unknown type '" + decl->type + "'");

        symTable.define(decl->name, type);
    }

    void analyzeAssign(AssignNode* assign) {
        if (!symTable.exists(assign->name))
            throw std::runtime_error("Variable '" + assign->name + "' not declared");

        VarType exprType = analyzeExpr(assign->expr.get());
        Symbol* sym = symTable.lookup(assign->name);

        if (exprType != VarType::UNDEFINED && sym->type != exprType) {
            throw std::runtime_error("Type mismatch in assignment to '" +
                                     assign->name + "'");
        }
        symTable.initialize(assign->name);
    }

    void analyzePrint(PrintNode* print) {
        analyzeExpr(print->expr.get());
    }

    void analyzeIf(IfNode* ifNode) {
        VarType condType = analyzeExpr(ifNode->condition.get());
        if (condType != VarType::BOOL && condType != VarType::INT &&
            condType != VarType::UNDEFINED) {
            throw std::runtime_error("If condition must be boolean or integer");
        }
        if (ifNode->thenBranch) analyzeBlock(ifNode->thenBranch.get());
        if (ifNode->elseBranch) analyzeBlock(ifNode->elseBranch.get());
    }

    void analyzeWhile(WhileNode* whileNode) {
        VarType condType = analyzeExpr(whileNode->condition.get());
        if (condType != VarType::BOOL && condType != VarType::INT &&
            condType != VarType::UNDEFINED) {
            throw std::runtime_error("While condition must be boolean or integer");
        }
        if (whileNode->body) analyzeBlock(whileNode->body.get());
    }

    void analyzeBlock(BlockNode* block) {
        for (auto& stmt : block->statements)
            analyzeStatement(stmt.get());
    }

    VarType analyzeExpr(ExprNode* expr) {
        if (dynamic_cast<NumberNode*>(expr)) return VarType::INT;
        if (dynamic_cast<BoolNode*>(expr))   return VarType::BOOL;
        if (dynamic_cast<StringNode*>(expr)) return VarType::STRING;

        if (auto id = dynamic_cast<IdentifierNode*>(expr)) {
            if (!symTable.exists(id->name))
                throw std::runtime_error("Variable '" + id->name + "' not declared");
            Symbol* sym = symTable.lookup(id->name);
            if (!sym->initialized)
                throw std::runtime_error("Variable '" + id->name +
                                         "' used before initialization");
            return sym->type;
        }

        if (auto unary = dynamic_cast<UnaryOpNode*>(expr)) {
            VarType t = analyzeExpr(unary->operand.get());
            if (t != VarType::INT && t != VarType::UNDEFINED)
                throw std::runtime_error("Unary '" + unary->op +
                                         "' requires integer operand");
            return VarType::INT;
        }

        if (auto bin = dynamic_cast<BinaryOpNode*>(expr)) {
            VarType l = analyzeExpr(bin->left.get());
            VarType r = analyzeExpr(bin->right.get());

            const std::string& op = bin->op;

            // Arithmetic: INT op INT -> INT
            if (op == "+" || op == "-" || op == "*" || op == "/") {
                if ((l != VarType::INT && l != VarType::UNDEFINED) ||
                    (r != VarType::INT && r != VarType::UNDEFINED)) {
                    throw std::runtime_error("Arithmetic operation requires integers");
                }
                return VarType::INT;
            }

            // Ordering: INT op INT -> BOOL
            if (op == ">" || op == "<" || op == ">=" || op == "<=") {
                if ((l != VarType::INT && l != VarType::UNDEFINED) ||
                    (r != VarType::INT && r != VarType::UNDEFINED)) {
                    throw std::runtime_error("Ordering comparison requires integers");
                }
                return VarType::BOOL;
            }

            // Equality: same type -> BOOL
            if (op == "==" || op == "!=") {
                if (l != r && l != VarType::UNDEFINED && r != VarType::UNDEFINED)
                    throw std::runtime_error("Equality comparison on mismatched types");
                return VarType::BOOL;
            }

            return VarType::UNDEFINED;
        }

        return VarType::UNDEFINED;
    }
};

#endif
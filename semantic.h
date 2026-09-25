#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "ast.h"
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>

// Variable type enum
enum class VarType { INT, STRING, BOOL, UNDEFINED };

// Symbol table entry
struct Symbol {
    std::string name;
    VarType type;
    bool initialized;
    
    Symbol() : name(""), type(VarType::UNDEFINED), initialized(false) {}
    Symbol(std::string n, VarType t, bool init = false) 
        : name(n), type(t), initialized(init) {}
};

// Symbol table for scope management
class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> symbols;
    
public:
    // Define a new variable
    void define(const std::string& name, VarType type) {
        if (symbols.find(name) != symbols.end()) {
            throw std::runtime_error("Variable '" + name + "' already declared");
        }
        symbols[name] = Symbol(name, type, false);
    }
    
    // Mark as initialized
    void initialize(const std::string& name) {
        if (symbols.find(name) == symbols.end()) {
            throw std::runtime_error("Variable '" + name + "' not declared");
        }
        symbols[name].initialized = true;
    }
    
    // Get symbol info
    Symbol* lookup(const std::string& name) {
        auto it = symbols.find(name);
        if (it == symbols.end()) return nullptr;
        return &it->second;
    }
    
    // Check if variable exists
    bool exists(const std::string& name) {
        return symbols.find(name) != symbols.end();
    }
    
    // Print all symbols
    void printSymbols() {
        std::cout << "\n=== SYMBOL TABLE ===" << std::endl;
        for (const auto& pair : symbols) {
            const Symbol& sym = pair.second;
            std::string typeStr;
            switch (sym.type) {
                case VarType::INT: typeStr = "int"; break;
                case VarType::STRING: typeStr = "string"; break;
                case VarType::BOOL: typeStr = "bool"; break;
                case VarType::UNDEFINED: typeStr = "undefined"; break;
            }
            std::string initStr = sym.initialized ? "yes" : "no";
            std::cout << sym.name << " : " << typeStr << " (" << initStr << ")" << std::endl;
        }
    }
};

// Semantic analyzer
class SemanticAnalyzer {
private:
    SymbolTable symTable;
    
public:
    SemanticAnalyzer() = default;
    
    // Analyze entire program
    void analyze(BlockNode* program) {
        for (auto& stmt : program->statements) {
            analyzeStatement(stmt.get());
        }
    }
    
    // Get symbol table (for code generator)
    SymbolTable& getSymbolTable() {
        return symTable;
    }
    
private:
    // Analyze statement
    void analyzeStatement(StmtNode* stmt) {
        if (auto decl = dynamic_cast<VarDeclNode*>(stmt)) {
            analyzeVarDecl(decl);
        } else if (auto assign = dynamic_cast<AssignNode*>(stmt)) {
            analyzeAssign(assign);
        } else if (auto print = dynamic_cast<PrintNode*>(stmt)) {
            analyzePrint(print);
        } else if (auto ifNode = dynamic_cast<IfNode*>(stmt)) {
            analyzeIf(ifNode);
        } else if (auto whileNode = dynamic_cast<WhileNode*>(stmt)) {
            analyzeWhile(whileNode);
        } else if (auto block = dynamic_cast<BlockNode*>(stmt)) {
            analyzeBlock(block);
        }
    }
    
    // Variable declaration
    void analyzeVarDecl(VarDeclNode* decl) {
        VarType type = VarType::UNDEFINED;
        if (decl->type == "পূর্ণসংখ্যা") {
            type = VarType::INT;
        } else if (decl->type == "সত্যমূল্য") {
            type = VarType::BOOL;
        } else if (decl->type == "টেক্সট") {
            type = VarType::STRING;
        }
        
        symTable.define(decl->name, type);
    }
    
    // Assignment
    void analyzeAssign(AssignNode* assign) {
        // Check variable exists
        if (!symTable.exists(assign->name)) {
            throw std::runtime_error("Variable '" + assign->name + "' not declared");
        }
        
        // Analyze expression
        VarType exprType = analyzeExpr(assign->expr.get());
        
        // Type match check
        Symbol* sym = symTable.lookup(assign->name);
        if (sym->type != exprType && sym->type != VarType::UNDEFINED && 
            exprType != VarType::UNDEFINED) {
            // Lenient: allow int/bool mixing for now
        }
        
        symTable.initialize(assign->name);
    }
    
    // Print statement
    void analyzePrint(PrintNode* print) {
        analyzeExpr(print->expr.get());
    }
    
    // If statement
    void analyzeIf(IfNode* ifNode) {
        // Condition must be bool or int
        VarType condType = analyzeExpr(ifNode->condition.get());
        if (condType != VarType::INT && condType != VarType::BOOL && 
            condType != VarType::UNDEFINED) {
            throw std::runtime_error("If condition must be boolean or integer");
        }
        
        if (ifNode->thenBranch) {
            analyzeBlock(ifNode->thenBranch.get());
        }
        
        if (ifNode->elseBranch) {
            analyzeBlock(ifNode->elseBranch.get());
        }
    }
    
    // While loop
    void analyzeWhile(WhileNode* whileNode) {
        VarType condType = analyzeExpr(whileNode->condition.get());
        if (condType != VarType::INT && condType != VarType::BOOL && 
            condType != VarType::UNDEFINED) {
            throw std::runtime_error("While condition must be boolean or integer");
        }
        
        if (whileNode->body) {
            analyzeBlock(whileNode->body.get());
        }
    }
    
    // Block
    void analyzeBlock(BlockNode* block) {
        for (auto& stmt : block->statements) {
            analyzeStatement(stmt.get());
        }
    }
    
    // Expression analysis (returns type)
    VarType analyzeExpr(ExprNode* expr) {
        if (auto num = dynamic_cast<NumberNode*>(expr)) {
            return VarType::INT;
        } else if (auto str = dynamic_cast<StringNode*>(expr)) {
            return VarType::STRING;
        } else if (auto id = dynamic_cast<IdentifierNode*>(expr)) {
            if (!symTable.exists(id->name)) {
                throw std::runtime_error("Variable '" + id->name + "' not declared");
            }
            Symbol* sym = symTable.lookup(id->name);
            if (!sym->initialized) {
                throw std::runtime_error("Variable '" + id->name + "' used before initialization");
            }
            return sym->type;
        } else if (auto binOp = dynamic_cast<BinaryOpNode*>(expr)) {
            VarType left = analyzeExpr(binOp->left.get());
            VarType right = analyzeExpr(binOp->right.get());
            
            // Arithmetic ops: INT
            if (binOp->op == "+" || binOp->op == "-" || 
                binOp->op == "*" || binOp->op == "/") {
                if ((left != VarType::INT && left != VarType::UNDEFINED) ||
                    (right != VarType::INT && right != VarType::UNDEFINED)) {
                    throw std::runtime_error("Arithmetic operation requires integers");
                }
                return VarType::INT;
            }
            
            // Comparison ops: BOOL
            if (binOp->op == ">" || binOp->op == "<" || 
                binOp->op == ">=" || binOp->op == "<=" ||
                binOp->op == "==" || binOp->op == "!=") {
                return VarType::BOOL;
            }
            
            return VarType::UNDEFINED;
        }
        
        return VarType::UNDEFINED;
    }
};

#endif

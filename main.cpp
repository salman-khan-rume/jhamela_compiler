#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <climits>

using namespace std;

// ================= TOKEN TYPES =================

enum class TokenType {
    SOMKHA, BUL, JODI, NAHLE, JOKHON, DEKHAO, SATYA, MITHYA,
    ID, NUMBER, STRING_LITERAL,
    PLUS, MINUS, MUL, DIV, ASSIGN,
    EQUAL_EQUAL, NOT_EQUAL, GREATER, LESS, GTE, LTE,
    AND, OR, NOT,
    LPAREN, RPAREN, LBRACE, RBRACE, SEMI,
    EOF_TOKEN
};

enum class VarType { INT, BOOL, STRING };

// ================= TOKEN CLASS =================

class Token {
public:
    TokenType type;
    string lexeme;
    int line;
    
    Token(TokenType type, string lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}
    
    string typeToString() const {
        switch(type) {
            case TokenType::SOMKHA: return "SOMKHA";
            case TokenType::BUL: return "BUL";
            case TokenType::JODI: return "JODI";
            case TokenType::NAHLE: return "NAHLE";
            case TokenType::JOKHON: return "JOKHON";
            case TokenType::DEKHAO: return "DEKHAO";
            case TokenType::SATYA: return "SATYA";
            case TokenType::MITHYA: return "MITHYA";
            case TokenType::ID: return "ID";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::STRING_LITERAL: return "STRING";
            case TokenType::PLUS: return "PLUS";
            case TokenType::MINUS: return "MINUS";
            case TokenType::MUL: return "MUL";
            case TokenType::DIV: return "DIV";
            case TokenType::ASSIGN: return "ASSIGN";
            case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
            case TokenType::NOT_EQUAL: return "NOT_EQUAL";
            case TokenType::GREATER: return "GREATER";
            case TokenType::LESS: return "LESS";
            case TokenType::GTE: return "GTE";
            case TokenType::LTE: return "LTE";
            case TokenType::AND: return "AND";
            case TokenType::OR: return "OR";
            case TokenType::NOT: return "NOT";
            case TokenType::LPAREN: return "LPAREN";
            case TokenType::RPAREN: return "RPAREN";
            case TokenType::LBRACE: return "LBRACE";
            case TokenType::RBRACE: return "RBRACE";
            case TokenType::SEMI: return "SEMI";
            case TokenType::EOF_TOKEN: return "EOF";
            default: return "UNKNOWN";
        }
    }
    
    string toString() const {
        return typeToString() + " : " + lexeme;
    }
};

// ================= AST NODES =================

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class NumberNode : public ASTNode {
public:
    int value;
    NumberNode(int value) : value(value) {}
};

class StringNode : public ASTNode {
public:
    string value;
    StringNode(string value) : value(value) {}
};

class BoolNode : public ASTNode {
public:
    bool value;
    BoolNode(bool value) : value(value) {}
};

class VarNode : public ASTNode {
public:
    string name;
    VarNode(string name) : name(name) {}
};

class BinOpNode : public ASTNode {
public:
    ASTNode *left;
    string op;
    ASTNode *right;
    
    BinOpNode(ASTNode *left, string op, ASTNode *right)
        : left(left), op(op), right(right) {}
};

class UnaryOpNode : public ASTNode {
public:
    string op;
    ASTNode *expr;
    
    UnaryOpNode(string op, ASTNode *expr)
        : op(op), expr(expr) {}
};

class AssignNode : public ASTNode {
public:
    VarType type;
    string name;
    ASTNode *expr;
    
    AssignNode(VarType type, string name, ASTNode *expr)
        : type(type), name(name), expr(expr) {}
};

class PrintNode : public ASTNode {
public:
    ASTNode *expr;
    PrintNode(ASTNode *expr) : expr(expr) {}
};

class IfNode : public ASTNode {
public:
    ASTNode *condition;
    vector<ASTNode *> thenBody;
    vector<ASTNode *> elseBody;
    
    IfNode(ASTNode *condition, vector<ASTNode *> thenBody, vector<ASTNode *> elseBody = {})
        : condition(condition), thenBody(thenBody), elseBody(elseBody) {}
};

class WhileNode : public ASTNode {
public:
    ASTNode *condition;
    vector<ASTNode *> body;
    
    WhileNode(ASTNode *condition, vector<ASTNode *> body)
        : condition(condition), body(body) {}
};

// ================= SYMBOL TABLE =================

struct Symbol {
    VarType type;
    string strValue;
    int intValue;
    bool boolValue;
};

class SymbolTable {
public:
    unordered_map<string, Symbol> table;
    
    void set(string name, VarType type, int intVal = 0, bool boolVal = false, string strVal = "") {
        table[name] = {type, strVal, intVal, boolVal};
    }
    
    bool contains(string name) {
        return table.find(name) != table.end();
    }
    
    Symbol get(string name) {
        return table[name];
    }
};

// ================= SEMANTIC ANALYZER =================

class SemanticAnalyzer {
private:
    SymbolTable &symbolTable;
    
public:
    SemanticAnalyzer(SymbolTable &st) : symbolTable(st) {}
    
    void analyze(vector<ASTNode *> &nodes) {
        for(auto node : nodes) {
            evaluate(node);
        }
    }
    
    int evaluate(ASTNode *node) {
        if(auto n = dynamic_cast<NumberNode *>(node)) {
            return n->value;
        }
        
        if(auto v = dynamic_cast<VarNode *>(node)) {
            if(!symbolTable.contains(v->name)) {
                throw runtime_error("Undefined variable: " + v->name);
            }
            return symbolTable.get(v->name).intValue;
        }
        
        if(auto b = dynamic_cast<BinOpNode *>(node)) {
            int left = evaluate(b->left);
            int right = evaluate(b->right);
            
            if(b->op == "+") return left + right;
            if(b->op == "-") return left - right;
            if(b->op == "*") return left * right;
            if(b->op == "/") {
                if(right == 0) throw runtime_error("Division by zero");
                return left / right;
            }
            if(b->op == ">") return left > right;
            if(b->op == "<") return left < right;
            if(b->op == ">=") return left >= right;
            if(b->op == "<=") return left <= right;
            if(b->op == "==") return left == right;
            if(b->op == "!=") return left != right;
            
            throw runtime_error("Unknown operator: " + b->op);
        }
        
        if(auto a = dynamic_cast<AssignNode *>(node)) {
            int value = evaluate(a->expr);
            symbolTable.set(a->name, a->type, value);
            return value;
        }
        
        return 0;
    }
    
    static int convertBanglaNumber(string s) {
        vector<string> bn = {"०", "१", "२", "३", "४", "५", "६", "७", "८", "९"};
        string result = "";
        
        for(int i = 0; i < s.size();) {
            bool found = false;
            for(int d = 0; d <= 9; d++) {
                if(s.substr(i, bn[d].size()) == bn[d]) {
                    result += char('0' + d);
                    i += bn[d].size();
                    found = true;
                    break;
                }
            }
            if(!found) i++;
        }
        
        if(result.empty()) return 0;
        return stoi(result);
    }
};

// ================= LEXER =================

class Lexer {
private:
    string source;
    vector<Token> tokens;
    int start = 0, current = 0, line = 1;
    
    unordered_map<string, TokenType> keywords = {
        {"সংখ্যা", TokenType::SOMKHA},
        {"বুল", TokenType::BUL},
        {"যদি", TokenType::JODI},
        {"নাহলে", TokenType::NAHLE},
        {"যখন", TokenType::JOKHON},
        {"দেখাও", TokenType::DEKHAO},
        {"সত্য", TokenType::SATYA},
        {"মিথ্যা", TokenType::MITHYA}
    };
    
public:
    Lexer(string source) : source(source) {}
    
    vector<Token> scanTokens() {
        while(!isAtEnd()) {
            start = current;
            scanToken();
        }
        tokens.push_back(Token(TokenType::EOF_TOKEN, "", line));
        return tokens;
    }
    
private:
    void scanToken() {
        unsigned char c = advance();
        
        switch(c) {
            case '(': addToken(TokenType::LPAREN); break;
            case ')': addToken(TokenType::RPAREN); break;
            case '{': addToken(TokenType::LBRACE); break;
            case '}': addToken(TokenType::RBRACE); break;
            case ';': addToken(TokenType::SEMI); break;
            case '+': addToken(TokenType::PLUS); break;
            case '-': addToken(TokenType::MINUS); break;
            case '*': addToken(TokenType::MUL); break;
            case '/': addToken(TokenType::DIV); break;
            case ' ': case '\r': case '\t': break;
            case '\n': line++; break;
            case '"': stringLiteral(); break;
            case '=':
                if(peek() == '=') { advance(); addToken(TokenType::EQUAL_EQUAL); }
                else addToken(TokenType::ASSIGN);
                break;
            case '!':
                if(peek() == '=') { advance(); addToken(TokenType::NOT_EQUAL); }
                else addToken(TokenType::NOT);
                break;
            case '>':
                if(peek() == '=') { advance(); addToken(TokenType::GTE); }
                else addToken(TokenType::GREATER);
                break;
            case '<':
                if(peek() == '=') { advance(); addToken(TokenType::LTE); }
                else addToken(TokenType::LESS);
                break;
            case '&':
                if(peek() == '&') { advance(); addToken(TokenType::AND); }
                break;
            case '|':
                if(peek() == '|') { advance(); addToken(TokenType::OR); }
                break;
            default:
                if(isdigit(c)) number();
                else if(c >= 224) identifier();
                else if(isalpha(c)) identifier();
                break;
        }
    }
    
    void stringLiteral() {
        while(peek() != '"' && !isAtEnd()) {
            if(peek() == '\n') line++;
            advance();
        }
        if(isAtEnd()) throw runtime_error("Unterminated string");
        advance();
        addToken(TokenType::STRING_LITERAL);
    }
    
    void identifier() {
        while(isalnum(peek()) || (unsigned char)peek() >= 128) advance();
        string text = source.substr(start, current - start);
        addToken(keywords.count(text) ? keywords[text] : TokenType::ID);
    }
    
    void number() {
        while(isdigit(peek()) || (unsigned char)peek() >= 128) advance();
        addToken(TokenType::NUMBER);
    }
    
    char advance() { return source[current++]; }
    char peek() { return isAtEnd() ? '\0' : source[current]; }
    bool isAtEnd() { return current >= source.size(); }
    void addToken(TokenType type) {
        tokens.push_back(Token(type, source.substr(start, current - start), line));
    }
};

// ================= PARSER =================

class Parser {
private:
    vector<Token> tokens;
    int current = 0;
    
public:
    Parser(vector<Token> tokens) : tokens(tokens) {}
    
    vector<ASTNode *> parse() {
        vector<ASTNode *> nodes;
        while(!isAtEnd()) nodes.push_back(statement());
        return nodes;
    }
    
private:
    ASTNode *statement() {
        if(match({TokenType::SOMKHA})) return numberDeclaration();
        if(match({TokenType::BUL})) return boolDeclaration();
        if(match({TokenType::DEKHAO})) return printStatement();
        if(match({TokenType::JODI})) return ifStatement();
        if(match({TokenType::JOKHON})) return whileStatement();
        throw runtime_error("Expected statement");
    }
    
    ASTNode *numberDeclaration() {
        Token name = consume(TokenType::ID, "Expected var name");
        consume(TokenType::ASSIGN, "Expected '='");
        ASTNode *expr = expression();
        consume(TokenType::SEMI, "Expected ';'");
        return new AssignNode(VarType::INT, name.lexeme, expr);
    }
    
    ASTNode *boolDeclaration() {
        Token name = consume(TokenType::ID, "Expected var name");
        consume(TokenType::ASSIGN, "Expected '='");
        ASTNode *expr = expression();
        consume(TokenType::SEMI, "Expected ';'");
        return new AssignNode(VarType::BOOL, name.lexeme, expr);
    }
    
    ASTNode *printStatement() {
        ASTNode *expr = expression();
        consume(TokenType::SEMI, "Expected ';'");
        return new PrintNode(expr);
    }
    
    ASTNode *ifStatement() {
        consume(TokenType::LPAREN, "Expected '('");
        ASTNode *cond = expression();
        consume(TokenType::RPAREN, "Expected ')'");
        consume(TokenType::LBRACE, "Expected '{'");
        
        vector<ASTNode *> thenBody;
        while(!check(TokenType::RBRACE) && !isAtEnd()) thenBody.push_back(statement());
        consume(TokenType::RBRACE, "Expected '}'");
        
        vector<ASTNode *> elseBody;
        if(match({TokenType::NAHLE})) {
            consume(TokenType::LBRACE, "Expected '{'");
            while(!check(TokenType::RBRACE) && !isAtEnd()) elseBody.push_back(statement());
            consume(TokenType::RBRACE, "Expected '}'");
        }
        
        return new IfNode(cond, thenBody, elseBody);
    }
    
    ASTNode *whileStatement() {
        consume(TokenType::LPAREN, "Expected '('");
        ASTNode *cond = expression();
        consume(TokenType::RPAREN, "Expected ')'");
        consume(TokenType::LBRACE, "Expected '{'");
        
        vector<ASTNode *> body;
        while(!check(TokenType::RBRACE) && !isAtEnd()) body.push_back(statement());
        consume(TokenType::RBRACE, "Expected '}'");
        
        return new WhileNode(cond, body);
    }
    
    ASTNode *expression() { return logicalOr(); }
    
    ASTNode *logicalOr() {
        ASTNode *node = logicalAnd();
        while(match({TokenType::OR})) {
            string op = previous().lexeme;
            node = new BinOpNode(node, op, logicalAnd());
        }
        return node;
    }
    
    ASTNode *logicalAnd() {
        ASTNode *node = equality();
        while(match({TokenType::AND})) {
            string op = previous().lexeme;
            node = new BinOpNode(node, op, equality());
        }
        return node;
    }
    
    ASTNode *equality() {
        ASTNode *node = comparison();
        while(match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
            string op = previous().lexeme;
            node = new BinOpNode(node, op, comparison());
        }
        return node;
    }
    
    ASTNode *comparison() {
        ASTNode *node = additive();
        while(match({TokenType::GREATER, TokenType::LESS, TokenType::GTE, TokenType::LTE})) {
            string op = previous().lexeme;
            node = new BinOpNode(node, op, additive());
        }
        return node;
    }
    
    ASTNode *additive() {
        ASTNode *node = multiplicative();
        while(match({TokenType::PLUS, TokenType::MINUS})) {
            string op = previous().lexeme;
            node = new BinOpNode(node, op, multiplicative());
        }
        return node;
    }
    
    ASTNode *multiplicative() {
        ASTNode *node = unary();
        while(match({TokenType::MUL, TokenType::DIV})) {
            string op = previous().lexeme;
            node = new BinOpNode(node, op, unary());
        }
        return node;
    }
    
    ASTNode *unary() {
        if(match({TokenType::NOT, TokenType::MINUS})) {
            return new UnaryOpNode(previous().lexeme, unary());
        }
        return primary();
    }
    
    ASTNode *primary() {
        if(match({TokenType::NUMBER})) {
            return new NumberNode(SemanticAnalyzer::convertBanglaNumber(previous().lexeme));
        }
        if(match({TokenType::STRING_LITERAL})) {
            string val = previous().lexeme;
            return new StringNode(val.substr(1, val.length() - 2));
        }
        if(match({TokenType::SATYA})) return new BoolNode(true);
        if(match({TokenType::MITHYA})) return new BoolNode(false);
        if(match({TokenType::ID})) return new VarNode(previous().lexeme);
        if(match({TokenType::LPAREN})) {
            ASTNode *expr = expression();
            consume(TokenType::RPAREN, "Expected ')'");
            return expr;
        }
        throw runtime_error("Expected expression");
    }
    
    bool match(initializer_list<TokenType> types) {
        for(TokenType type : types) {
            if(check(type)) { advance(); return true; }
        }
        return false;
    }
    
    bool check(TokenType type) { return !isAtEnd() && peek().type == type; }
    Token advance() { if(!isAtEnd()) current++; return previous(); }
    bool isAtEnd() { return peek().type == TokenType::EOF_TOKEN; }
    Token peek() { return tokens[current]; }
    Token previous() { return tokens[current - 1]; }
    Token consume(TokenType type, string msg) {
        if(check(type)) return advance();
        throw runtime_error(msg);
    }
};

// ================= CODE GENERATOR =================

class CodeGenerator {
private:
    ofstream &out;
    int indentLevel = 0;
    
public:
    CodeGenerator(ofstream &out) : out(out) {}
    
    void generate(vector<ASTNode *> &nodes) {
        out << "#!/usr/bin/env python3\n\n";
        
        for(auto node : nodes) {
            generateStatement(node);
        }
    }
    
private:
    string getIndent() {
        return string(indentLevel * 4, ' ');
    }
    
    void generateStatement(ASTNode *node) {
        if(auto a = dynamic_cast<AssignNode *>(node)) {
            out << getIndent() << a->name << " = " << generateExpression(a->expr) << "\n";
        }
        else if(auto p = dynamic_cast<PrintNode *>(node)) {
            out << getIndent() << "print(" << generateExpression(p->expr) << ")\n";
        }
        else if(auto i = dynamic_cast<IfNode *>(node)) {
            out << getIndent() << "if " << generateExpression(i->condition) << ":\n";
            indentLevel++;
            for(auto stmt : i->thenBody) generateStatement(stmt);
            indentLevel--;
            
            if(!i->elseBody.empty()) {
                out << getIndent() << "else:\n";
                indentLevel++;
                for(auto stmt : i->elseBody) generateStatement(stmt);
                indentLevel--;
            }
        }
        else if(auto w = dynamic_cast<WhileNode *>(node)) {
            out << getIndent() << "while " << generateExpression(w->condition) << ":\n";
            indentLevel++;
            for(auto stmt : w->body) generateStatement(stmt);
            indentLevel--;
        }
    }
    
    string generateExpression(ASTNode *node) {
        if(auto n = dynamic_cast<NumberNode *>(node)) {
            return to_string(n->value);
        }
        if(auto s = dynamic_cast<StringNode *>(node)) {
            return "\"" + s->value + "\"";
        }
        if(auto b = dynamic_cast<BoolNode *>(node)) {
            return b->value ? "True" : "False";
        }
        if(auto v = dynamic_cast<VarNode *>(node)) {
            return v->name;
        }
        if(auto bin = dynamic_cast<BinOpNode *>(node)) {
            string op = bin->op;
            if(op == "&&") op = "and";
            if(op == "||") op = "or";
            return "(" + generateExpression(bin->left) + " " + op + " " + generateExpression(bin->right) + ")";
        }
        if(auto un = dynamic_cast<UnaryOpNode *>(node)) {
            string op = un->op;
            if(op == "!") op = "not ";
            return op + generateExpression(un->expr);
        }
        return "";
    }
};

// ================= MAIN =================

int main() {
    ifstream in("input.jh");
    ofstream outTxt("output.txt");
    ofstream outPy("output.py");
    
    try {
        if(!in.is_open()) throw runtime_error("Cannot open input.jh");
        
        stringstream buffer;
        buffer << in.rdbuf();
        string source = buffer.str();
        
        Lexer lexer(source);
        vector<Token> tokens = lexer.scanTokens();
        
        Parser parser(tokens);
        vector<ASTNode *> nodes = parser.parse();
        
        SymbolTable st;
        SemanticAnalyzer analyzer(st);
        analyzer.analyze(nodes);
        
        CodeGenerator generator(outPy);
        generator.generate(nodes);
        
        outTxt << "=== TOKENS ===\n";
        for(auto t : tokens) outTxt << t.toString() << "\n";
        
        outTxt << "\n=== COMPILATION SUCCESS ===\n";
        outTxt << "Python code generated to: output.py\n";
        
        cout << "JHAMELA Compiler: Success!\n";
        cout << "Output: output.py\n";
    }
    catch(exception &e) {
        cerr << "Compiler Error: " << e.what() << "\n";
        outTxt << "Compiler Error: " << e.what() << "\n";
    }
    
    return 0;
}
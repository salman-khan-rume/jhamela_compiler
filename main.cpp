#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"

using namespace std;

int main() {
    ifstream in("input.jh");
    if (!in.is_open()) {
        cerr << "Error: Cannot open input.jh\n";
        return 1;
    }

    stringstream buffer;
    buffer << in.rdbuf();
    string source = buffer.str();

    Lexer lexer(source);
    vector<Token> tokens = lexer.scanTokens();

    cout << "=== TOKENS ===\n";
    for (const auto& t : tokens) {
        if (t.type != TokenType::EOF_TOKEN) cout << t.lexeme << " ";
    }
    cout << "\n\n=== AST ===\n";

    Parser parser(tokens);
    vector<ASTNode*> ast = parser.parse();

    for (const auto& node : ast) {
        if (node) node->print();
    }

    return 0;
}
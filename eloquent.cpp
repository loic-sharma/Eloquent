#include <cstdlib>
#include <iostream>
#include <string>
#include <cassert>
#include "ast.h"
#include "parser.h"
#include "lexer.yy.h"

void* ParseAlloc(void* (*allocProc)(size_t));
void Parse(void* parser, int token, const char *tokenInfo, Node **ast);
void ParseFree(void* parser, void(*freeProc)(void*));

#include <unordered_map>

int *evaluate(Node *ast, std::unordered_map<std::string, int> *symbols = nullptr) {
    if (ast == nullptr) return nullptr;

    if (symbols == nullptr) {
        symbols = new std::unordered_map<std::string, int>;
    }

    std::string value(ast->value);

    if (value == "=") {
        (*symbols)[ast->left->value] = *evaluate(ast->right, symbols);

        return nullptr;
    }
    else {
        int *left = (ast->left) ? evaluate(ast->left, symbols) : nullptr;
        int *right = (ast->right) ? evaluate(ast->right, symbols) : nullptr;

        if (value == "*") {
            return new int(*left * *right);
        }
        else if (value == "/") {
            return new int(*left / *right);
        }
        else if (value == "+") {
            return new int(*left + *right);
        }
        else if (value == "-") {
            return new int(*left - *right);
        }
        else if (value == "%") {
            return new int(*left % *right);   
        }
        else if (value == "__compound") {
            evaluate(ast->left);

            return evaluate(ast->right);
        }
        else {
            if (ast->value and (*ast->value >= '0' and *ast->value <= '9')) {
                return new int(atoi(ast->value));
            }
            else {
                return new int((*symbols)[ast->value]);
            }
        }
    }
}

void manual() {
    void *parser = ParseAlloc(malloc);
    Node *AST = nullptr;

    Parse(parser, INTEGER, "15", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(AST->value == "15");
    assert(AST->left == nullptr);
    assert(AST->right == nullptr);

    assert(*evaluate(AST) == 15);

    std::cout << "---------------------\n";

    Parse(parser, INTEGER, "15", &AST);
    Parse(parser, ADD, "+", &AST);
    Parse(parser, INTEGER, "20", &AST);
    Parse(parser, MULT, "*", &AST);
    Parse(parser, INTEGER, "5", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(AST->value == "+");
    assert(AST->left->value == "15");
    assert(AST->right->value == "*");
    assert(AST->right->left->value == "20");
    assert(AST->right->right->value == "5"); 
    assert(AST->right->left->left == nullptr);
    assert(AST->right->left->right == nullptr);
    assert(AST->right->right->left == nullptr);
    assert(AST->right->right->right == nullptr);

    assert(*evaluate(AST) == 115);

    std::cout << "---------------------\n";

    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, ASSIGN, "=", &AST);
    Parse(parser, INTEGER, "15", &AST);
    Parse(parser, ADD, "+", &AST);
    Parse(parser, INTEGER, "20", &AST);
    Parse(parser, MULT, "*", &AST);
    Parse(parser, INTEGER, "5", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(AST->value == "=");
    assert(AST->left->value == "a");
    assert(AST->right->value == "+");
    assert(AST->right->left->value == "15");
    assert(AST->right->right->value == "*");
    assert(AST->right->right->left->value == "20");
    assert(AST->right->right->right->value == "5");

    assert(evaluate(AST) == nullptr);    

    std::cout << "---------------------\n";

    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, ASSIGN, "=", &AST);
    Parse(parser, INTEGER, "15", &AST);
    Parse(parser, SEMICOLON, ";", &AST);

    Parse(parser, IDENTIFIER, "b", &AST);
    Parse(parser, ASSIGN, "=", &AST);
    Parse(parser, INTEGER, "30", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    
    Parse(parser, IDENTIFIER, "c", &AST);
    Parse(parser, ASSIGN, "=", &AST);
    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, MULT, "*", &AST);
    Parse(parser, IDENTIFIER, "b", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(AST->value == "__compound");
    assert(AST->left->value == "__compound");
    assert(AST->right->value == "=");
    assert(AST->left->left->value == "=");
    assert(AST->left->right->value == "=");
    assert(AST->right->left->value == "c");
    assert(AST->right->right->value == "*");
    assert(AST->left->left->left->value == "a");
    assert(AST->left->left->right->value == "15");
    assert(AST->left->right->left->value == "b");
    assert(AST->left->right->right->value == "30");
    assert(AST->right->right->left->value == "a");
    assert(AST->right->right->right->value == "b");

    assert(evaluate(AST) == nullptr);

    std::cout << "---------------------\n";

    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, ASSIGN, "=", &AST);
    Parse(parser, INTEGER, "15", &AST);
    Parse(parser, SEMICOLON, ";", &AST);

    Parse(parser, IDENTIFIER, "b", &AST);
    Parse(parser, ASSIGN, "=", &AST);
    Parse(parser, INTEGER, "30", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    
    Parse(parser, IDENTIFIER, "c", &AST);
    Parse(parser, ASSIGN, "=", &AST);
    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, MULT, "*", &AST);
    Parse(parser, IDENTIFIER, "b", &AST);
    Parse(parser, SEMICOLON, ";", &AST);

    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, MULT, "*", &AST);
    Parse(parser, IDENTIFIER, "b", &AST);
    Parse(parser, SUB, "-", &AST);
    Parse(parser, IDENTIFIER, "c", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(*evaluate(AST) == (15 * 30 - (15*30)));

    ParseFree(parser, free);
}

/*
void parse(const string& commandLine) {
    // Set up the scanner
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE bufferState = yy_scan_string(commandLine.c_str(), scanner);
 
    // Set up the parser
    void *shellParser = ParseAlloc(malloc);
 
    int lexCode;
    bool validParse = true;
    do {
        lexCode = yylex(scanner);
        std::cout << "Lexed: " << yyget_text(scanner) << std::endl;
        Parse(shellParser, lexCode, yyget_text(scanner), &validParse);
    }
    while (lexCode > 0 && validParse);
 
    if (-1 == lexCode) {
        cerr << "The scanner encountered an error.\n";
    }

    if (!validParse) {
        cerr << "The parser encountered an error.\n";
    }
 
    // Cleanup the scanner and parser
    yy_delete_buffer(bufferState, scanner);
    yylex_destroy(scanner);
    ParseFree(shellParser, free);
}
*/
 
int main() {
    manual();

    return 0;
    /*
    string commandLine;
    while (getline(cin, commandLine)) {
        parse(commandLine);
    }
    return 0;
    */
}
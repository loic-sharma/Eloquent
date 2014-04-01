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

    bool own_symbols = false;

    if (symbols == nullptr) {
        symbols = new std::unordered_map<std::string, int>;

        own_symbols = true;
    }

    std::string value(ast->value);

    if (value == "=") {
        if (ast->left and ast->right) {
            int *value = evaluate(ast->right, symbols);

            if (value) {
                (*symbols)[ast->left->value] = *value;
            }
            else {
                std::cerr << "Value is a null pointer." << std::endl;
                exit(1);
            }

            return nullptr;
        }
        else {
            std::cerr << "No left or right child for assignment." << std::endl;
            exit(1);
        }
    }
    else {
        int left, right;
        int *left_ptr = evaluate(ast->left, symbols);
        int *right_ptr = evaluate(ast->right, symbols);

        if (left_ptr) {
            left = *left_ptr;

            delete left_ptr;
        }

        if (right_ptr) {
            right = *right_ptr;

            delete right_ptr;
        }

        if (value == "*") {
            return new int(left * right);
        }
        else if (value == "/") {
            return new int(left / right);
        }
        else if (value == "+") {
            return new int(left + right);
        }
        else if (value == "-") {
            return new int(left - right);
        }
        else if (value == "%") {
            return new int(left % right);   
        }
        else if (value == "__compound") {
            int *output = (right_ptr) ? new int(right) : nullptr;

            return output;
        }
        else {
            if (ast->value and (*ast->value >= '0' and *ast->value <= '9')) {
                return new int(atoi(ast->value));
            }
            else {
                if (symbols->find(ast->value) != symbols->end()) {
                    return new int((*symbols)[ast->value]);
                }
                else {
                    std::cerr << "Variable '" << ast->value << "' does not exist." << std::endl;
                    exit(1);
                }
            }
        }
    }

    if (own_symbols) {
        delete symbols;
    }
}
 
int main() {
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
    return 0;
}
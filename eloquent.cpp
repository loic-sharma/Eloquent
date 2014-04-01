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
#include <vector>
#include <stack>

struct Function {
    std::string name;
    std::vector<std::string> parameters;
    Node *content;
};

typedef std::unordered_map<std::string, int> Symbols;
typedef std::unordered_map<std::string, Function> Functions;

int *evaluate(Node *ast, Symbols *symbols = nullptr, Functions *functions = nullptr) {
    if (ast == nullptr) return nullptr;

    bool own_symbols = false, own_functions = false;

    if (symbols == nullptr) {
        symbols = new Symbols;

        own_symbols = true;
    }

    if (functions == nullptr) {
        functions = new Functions;

        own_functions = true;
    }

    std::string value(ast->value);

    if (value == "__function") {
        if (ast->right == nullptr) {
            return nullptr;
        }
        else if(ast->left == nullptr or ast->left->left == nullptr) {
            std::cerr << "Function has invalid prototype" << std::endl;
            exit(1);
        }
        else {
            std::vector<std::string> parameters;
            std::stack<Node *> nodes;

            nodes.push(ast->left->right);

            while (nodes.empty() == false) {
                Node *current = nodes.top();

                if (current->value[0] == ',') {
                    if (current->right) nodes.push(current->right);
                    if (current->left) nodes.push(current->left);
                }
                else {
                    parameters.push_back(current->value);

                    nodes.pop();
                }
            }

            Function function = {ast->left->left->value, parameters, ast->right};

            (*functions)[function.name] = function;
        }

        return nullptr;
    }
    else if (value == "__call") {
        auto it = functions->find(ast->left->value);

        if (it == functions->end()) {
            std::cerr << "Invalid function '" << ast->left->value << "'" << std::endl;
            exit(1);
        }

        Symbols *parameters = new Symbols;
        unsigned parameter_count = 0;

        std::stack<Node *> nodes;

        nodes.push(ast->right);

        while (nodes.empty() == false) {
            Node *current = nodes.top();

            if (current->value[0] == ',') {
                if (current->right) nodes.push(current->right);
                if (current->left) nodes.push(current->left);
            }
            else {
                std::string name(it->second.parameters[parameter_count++]);

                (*parameters)[name] = atoi(current->value);

                nodes.pop();
            }
        }

        int *result = evaluate(it->second.content, parameters, functions);

        delete parameters;

        return result;
    }
    else if(value == "__return") {
        return evaluate(ast->left, symbols, functions);
    }
    else if (value == "=") {
        if (ast->left and ast->right) {
            int *value = evaluate(ast->right, symbols, functions);

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
        int *left_ptr = evaluate(ast->left, symbols, functions);
        int *right_ptr = evaluate(ast->right, symbols, functions);

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

    if (own_functions) {
        delete functions;
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

    std::cout << "---------------------\n";


    Parse(parser, FUNCTION, "function", &AST);
    Parse(parser, IDENTIFIER, "double", &AST);
    Parse(parser, LPAREN, "(", &AST);
    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, RPAREN, ")", &AST);
    Parse(parser, LBRACE, "{", &AST);
    Parse(parser, RETURN, "return", &AST);
    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, MULT, "*", &AST);
    Parse(parser, INTEGER, "2", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, RBRACE, "}", &AST);

    Parse(parser, IDENTIFIER, "double", &AST);
    Parse(parser, LPAREN, "(", &AST);
    Parse(parser, INTEGER, "7", &AST);
    Parse(parser, RPAREN, ")", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(AST);
    assert(AST->left);
    assert(AST->right);
    assert(AST->value == "__compound");
    assert(AST->left->value == "__function");

    assert(AST->left->left->value == "__prototype");
    assert(AST->left->left->left->value == "double");
    assert(AST->left->left->right->value == "a");
    assert(AST->left->left->right->left == nullptr);
    assert(AST->left->left->right->right == nullptr);

    assert(AST->left->right->value == "__return");
    assert(AST->left->right->left->value == "*");
    assert(AST->left->right->left->left->value == "a");
    assert(AST->left->right->left->right->value == "2");

    assert(AST->right->value == "__call");
    assert(AST->right->left->value == "double");
    assert(AST->right->right->value == "7");

    assert(*evaluate(AST) == 14);

    ParseFree(parser, free);

    return 0;
}
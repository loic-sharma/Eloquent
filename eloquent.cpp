#include <cstdlib>
#include <iostream>
#include <string>
#include <cassert>

#include "ast.h"
#include "parser.h"
#include "virtual_machine.h"

void *ParseAlloc(void* (*allocProc)(size_t));
void Parse(void* parser, int token, const char *tokenInfo, Node **ast);
void ParseFree(void* parser, void(*freeProc)(void*));

int main() {
    VirtualMachine vm;

    void *parser = ParseAlloc(malloc);
    Node *AST = nullptr;

    Parse(parser, PRINT, "print", &AST);
    Parse(parser, INTEGER, "15", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    vm.execute(AST);

    /*
    //////////////////////////////////////////

    Parse(parser, INTEGER, "15", &AST);
    Parse(parser, SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(AST->value == "15");
    assert(AST->left == nullptr);
    assert(AST->right == nullptr);

    assert(*evaluate(AST) == 15);

    //////////////////////////////////////////

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

    //////////////////////////////////////////

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

    //////////////////////////////////////////

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

    //////////////////////////////////////////

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

    //////////////////////////////////////////

    Parse(parser, FUNCTION, "function", &AST);
    Parse(parser, IDENTIFIER, "double", &AST);
    Parse(parser, LPAREN, "(", &AST);
    Parse(parser, IDENTIFIER, "a", &AST);
    Parse(parser, COMMA, ",", &AST);
    Parse(parser, IDENTIFIER, "b", &AST);
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
    Parse(parser, COMMA, ",", &AST);
    Parse(parser, INTEGER, "0", &AST);
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

    assert(AST->left->left->right->value == ",");
    assert(AST->left->left->right->left->value == "a");
    assert(AST->left->left->right->right->value == "b");

    assert(AST->left->right->value == "__return");
    assert(AST->left->right->left->value == "*");
    assert(AST->left->right->left->left->value == "a");
    assert(AST->left->right->left->right->value == "2");

    assert(AST->right->value == "__call");
    assert(AST->right->left->value == "double");
    assert(AST->right->right->value == ",");
    assert(AST->right->right->left->value == "7");
    assert(AST->right->right->right->value == "0");

    assert(*evaluate(AST) == 14);
    */
    ParseFree(parser, free);

    return 0;
}
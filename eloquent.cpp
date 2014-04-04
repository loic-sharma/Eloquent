#include <cstdlib>
#include <iostream>
#include <string>
#include <cassert>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "virtual_machine.h"

void *ParseAlloc(void *(*allocProc)(size_t));
void Parse(void *parser, int token, const char *tokenInfo, Node **ast);
void ParseFree(void *parser, void(*freeProc)(void *));

int main() {
    Lexer lexer("1 + 2");

    lexer.lex();
    assert(lexer.current_token.type == T_INTEGER);
    assert(lexer.current_token.value == "1");
    assert(lexer.current_token.line == 1);

    lexer.lex();
    assert(lexer.current_token.type == T_ADD);
    assert(lexer.current_token.value == "+");
    assert(lexer.current_token.line == 1);

    lexer.lex();
    assert(lexer.current_token.type == T_INTEGER);
    assert(lexer.current_token.value == "2");
    assert(lexer.current_token.line == 1);

    lexer.lex();
    assert(lexer.current_token.type == T_EOF);
    assert(lexer.current_token.value == "");
    assert(lexer.current_token.line == 1);

    //////////////////////////////////////////

    VirtualMachine vm;

    void *parser = ParseAlloc(malloc);
    Node *AST = nullptr;

    Parse(parser, T_PRINT, "print", &AST);
    Parse(parser, T_INTEGER, "15", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    vm.execute(AST);

    //////////////////////////////////////////

    Parse(parser, T_INTEGER, "15", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(AST->value == "15");
    assert(AST->left == nullptr);
    assert(AST->right == nullptr);

    vm.execute(AST);

    //////////////////////////////////////////

    Parse(parser, T_INTEGER, "15", &AST);
    Parse(parser, T_ADD, "+", &AST);
    Parse(parser, T_INTEGER, "20", &AST);
    Parse(parser, T_MULT, "*", &AST);
    Parse(parser, T_INTEGER, "5", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
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

    vm.execute(AST);

    //////////////////////////////////////////

    Parse(parser, T_IDENTIFIER, "a", &AST);
    Parse(parser, T_ASSIGN, "=", &AST);
    Parse(parser, T_INTEGER, "15", &AST);
    Parse(parser, T_ADD, "+", &AST);
    Parse(parser, T_INTEGER, "20", &AST);
    Parse(parser, T_MULT, "*", &AST);
    Parse(parser, T_INTEGER, "5", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    assert(AST->value == "=");
    assert(AST->left->value == "a");
    assert(AST->right->value == "+");
    assert(AST->right->left->value == "15");
    assert(AST->right->right->value == "*");
    assert(AST->right->right->left->value == "20");
    assert(AST->right->right->right->value == "5");

    vm.execute(AST);   

    //////////////////////////////////////////

    Parse(parser, T_IDENTIFIER, "a", &AST);
    Parse(parser, T_ASSIGN, "=", &AST);
    Parse(parser, T_INTEGER, "15", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);

    Parse(parser, T_IDENTIFIER, "b", &AST);
    Parse(parser, T_ASSIGN, "=", &AST);
    Parse(parser, T_INTEGER, "30", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
    
    Parse(parser, T_IDENTIFIER, "c", &AST);
    Parse(parser, T_ASSIGN, "=", &AST);
    Parse(parser, T_IDENTIFIER, "a", &AST);
    Parse(parser, T_MULT, "*", &AST);
    Parse(parser, T_IDENTIFIER, "b", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
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

    vm.execute(AST);

    //////////////////////////////////////////

    Parse(parser, T_IDENTIFIER, "a", &AST);
    Parse(parser, T_ASSIGN, "=", &AST);
    Parse(parser, T_INTEGER, "15", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);

    Parse(parser, T_IDENTIFIER, "b", &AST);
    Parse(parser, T_ASSIGN, "=", &AST);
    Parse(parser, T_INTEGER, "30", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
    
    Parse(parser, T_IDENTIFIER, "c", &AST);
    Parse(parser, T_ASSIGN, "=", &AST);
    Parse(parser, T_IDENTIFIER, "a", &AST);
    Parse(parser, T_MULT, "*", &AST);
    Parse(parser, T_IDENTIFIER, "b", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);

    Parse(parser, T_IDENTIFIER, "a", &AST);
    Parse(parser, T_MULT, "*", &AST);
    Parse(parser, T_IDENTIFIER, "b", &AST);
    Parse(parser, T_SUB, "-", &AST);
    Parse(parser, T_IDENTIFIER, "c", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
    Parse(parser, 0, 0, &AST);

    vm.execute(AST);

    //////////////////////////////////////////

    Parse(parser, T_FUNCTION, "function", &AST);
    Parse(parser, T_IDENTIFIER, "double", &AST);
    Parse(parser, T_LPAREN, "(", &AST);
    Parse(parser, T_IDENTIFIER, "a", &AST);
    Parse(parser, T_COMMA, ",", &AST);
    Parse(parser, T_IDENTIFIER, "b", &AST);
    Parse(parser, T_RPAREN, ")", &AST);
    Parse(parser, T_LBRACE, "{", &AST);
    Parse(parser, T_RETURN, "return", &AST);
    Parse(parser, T_IDENTIFIER, "a", &AST);
    Parse(parser, T_MULT, "*", &AST);
    Parse(parser, T_INTEGER, "2", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
    Parse(parser, T_RBRACE, "}", &AST);

    Parse(parser, T_IDENTIFIER, "double", &AST);
    Parse(parser, T_LPAREN, "(", &AST);
    Parse(parser, T_INTEGER, "7", &AST);
    Parse(parser, T_COMMA, ",", &AST);
    Parse(parser, T_INTEGER, "0", &AST);
    Parse(parser, T_RPAREN, ")", &AST);
    Parse(parser, T_SEMICOLON, ";", &AST);
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

    vm.execute(AST);

    ParseFree(parser, free);

    return 0;
}
#include "parser.h"
#include "lexer.h"
#include "scanner.h"
#include "ast.h"

#include <cstdlib>

void *ParseAlloc(void *(*allocProc)(size_t));
void Parse(void *parser, int token, const char *tokenInfo, Node **ast);
void ParseFree(void *parser, void(*freeProc)(void *));

Node *Parser::parse(std::string input) {
    // Parse the script.
    Lexer lexer(input);

    void *parser = ParseAlloc(malloc);
    Node *AST = nullptr;

    bool empty = true;
    
    do {
        lexer.lex();
        
        if (lexer.token.type == T_EOF) {
            if (empty == false) {
                Parse(parser, 0, 0, &AST);
            }
        }
        else {
            empty = false;
            
            char *value = new char[lexer.token.value.length() + 1];
            std::strcpy(value, lexer.token.value.c_str());

            Parse(parser, lexer.token.type, value, &AST);
        }
    }
    while (lexer.token.type != T_EOF and lexer.token.type != T_UNKNOWN);

    return AST;
}
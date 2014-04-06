#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <cassert>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "virtual_machine.h"

void *ParseAlloc(void *(*allocProc)(size_t));
void Parse(void *parser, int token, const char *tokenInfo, Node **ast);
void ParseFree(void *parser, void(*freeProc)(void *));

std::string get_file_contents(const char *filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);

    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();

        return(contents);
    }

    throw(errno);
}

int main() {
    std::string code = get_file_contents("tests/simple_addition.l");

    Lexer lexer(code);

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

    VirtualMachine vm;

    vm.execute(AST);

    return 0;
}
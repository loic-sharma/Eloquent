#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <cassert>

#include "ast.h"
#include "lexer.h"
#include "scanner.h"
#include "compiler.h"
#include "virtual_machine.h"

void *ParseAlloc(void *(*allocProc)(size_t));
void Parse(void *parser, int token, const char *tokenInfo, Node **ast);
void ParseFree(void *parser, void(*freeProc)(void *));

std::string get_file_contents(const char *file) {
    std::ifstream in(file, std::ios::in | std::ios::binary);

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

void execute(const char *file) {
    std::string code = get_file_contents(file);

    // Parse the script.
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

    // Compile the syntax tree into bytecode.
    Compiler compiler;

    Program *program = compiler.compile(AST);

    // Execute the bytecode on our virtual machine.
    VirtualMachine vm;

    vm.execute(program);

    delete program;
}

int main() {
    execute("tests/comments.l");
    execute("tests/hello_world.l");
    execute("tests/arithmetic.l");
    execute("tests/conditional.l");

    return 0;
}
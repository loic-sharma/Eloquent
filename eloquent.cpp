#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <cassert>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "virtual_machine.h"


#include <iostream>
#include "value.h"


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

    Compiler compiler;

    Program *program = compiler.compile(AST);

    static std::string instructions[] = {
        "Jump",
        "Print",
        "Call",
        "Return",
        "Assign",
        "TrueJump",
        "FalseJump",
        "And",
        "Or",
        "Equals",
        "NEquals",
        "Increment",
        "Decrement",
        "Add",
        "Sub",
        "Mult",
        "Div",
        "Mod",
        "Identifier",
        "Value"
    };

    for (auto i = program->instructions->begin(); i != program->instructions->end(); ++i) {
        std::cout << instructions[i->type];

        if (i->type == Instruction::ValueType or i->type == Instruction::IdentifierType) {
            std::cout << " (" << i->value->to_string() << ')';
        }

        std::cout << std::endl;
    }

    // VirtualMachine vm;

    // vm.execute(AST);

    return 0;
}
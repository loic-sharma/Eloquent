
#include <fstream>
#include <string>
#include <cerrno>
#include <cassert>

#include "ast.h"
#include "parser.h"
#include "compiler.h"
#include "virtual_machine.h"

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

    // Parse the code into a syntax tree.
    Parser parser;

    Node *AST = parser.parse(code);

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
    execute("tests/iterations.l");

    return 0;
}
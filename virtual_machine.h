#ifndef __Eloquent_virtual_machine__
#define __Eloquent_virtual_machine__

#include <unordered_map>
#include <vector>
#include <string>

class Node;

struct Function {
    std::string name;
    std::vector<std::string> parameters;
    Node *content;
};

typedef std::unordered_map<std::string, int> Symbols;

class VirtualMachine {

public:
    void execute(Node *AST);

private:
    int *evaluate(Node *node, Symbols *symbols = nullptr);

    std::unordered_map<std::string, Function> functions;
};

#endif

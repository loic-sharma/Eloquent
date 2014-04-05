#ifndef __Eloquent_virtual_machine__
#define __Eloquent_virtual_machine__

#include <unordered_map>
#include <vector>
#include <string>

class Node;
class Value;

struct Function {
    std::string name;
    std::vector<std::string> parameters;
    Node *content;
};

typedef std::unordered_map<std::string, Value *> Symbols;

class VirtualMachine {

public:
    void execute(Node *AST);

private:
    Value *evaluate(Node *node, Symbols *symbols = nullptr);

    std::unordered_map<std::string, Function> functions;
};

#endif

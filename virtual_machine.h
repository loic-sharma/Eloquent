#ifndef __Eloquent_virtual_machine__
#define __Eloquent_virtual_machine__

#include <unordered_map>
#include <vector>
#include <string>

#include "compiler.h"

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
    void execute(Program *program);

private:
    Value *evaluate(Node *node, Symbols *symbols = nullptr);
    Value *evaluate(Instructions *instructions, Symbols *symbols = nullptr);

	enum State {
		NormalState,
		ReturnState,
	};

    std::unordered_map<std::string, Function> functions;
    State state;
};

#endif

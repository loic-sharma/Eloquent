//
//  compiler.h
//  Compiler
//
//  Created by Loic Sharma on 4/29/14.
//  Copyright (c) 2014 sharml. All rights reserved.
//

#ifndef __Compiler__Compiler__
#define __Compiler__Compiler__

#include <vector>
#include <string>
#include <unordered_map>

class Node;
class Value;

typedef struct {
    enum Type {
        JumpType,
        PrintType,
        CallType,
        ReturnType,
        AssignType,
        TrueJumpType,
        FalseJumpType,
        AndType,
        OrType,
        EqualsType,
        NEqualsType,
        IncrementType,
        DecrementType,
        AddType,
        SubType,
        MultType,
        DivType,
        ModType,
        IdentifierType,
        ValueType,
        EndType
    };

    Value *value;
    Type type;   
    long flags;
} Instruction;

typedef std::vector<Instruction> Instructions;
typedef std::unordered_map<std::string, Instructions *> Functions;

class Program {
public:
    Functions functions;
    Instructions *instructions;
};

class Compiler {

public:
    Program *compile(Node *ast);

private:
    Instructions *compile_node(Program *program, Node *node);
};

#endif /* defined(__Compiler__Compiler__) */

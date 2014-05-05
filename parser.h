//
//  parser.h
//  Compiler
//
//  Created by Loic Sharma on 5/4/14.
//  Copyright (c) 2014 sharml. All rights reserved.
//

#ifndef __Eloquent_parser__
#define __Eloquent_parser__

#include <string>

class Node;

class Parser {

public:
	Node *parse(std::string input);
};

#endif
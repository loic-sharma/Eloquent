#ifndef __Eloquent_lexer__
#define __Eloquent_lexer__

#include <istream>

#include "parser.h"

class Lexer {

public:
	Lexer(std::istream &input);

	unsigned int lex();

private:
	std::istream &input;
};

#endif

%include
{
	#include <iostream>
	#include <cassert>
	#include <stdlib.h>
	#include "ast.h"
}

%syntax_error
{
	std::cout << "ERROR!\n";
}

%extra_argument {Node **AST}

%token_type {const char *}
%default_type {Node *}

%right ASSIGN.
%right EQUALS.
%left ADD SUB.
%left MULT DIV MOD.

program ::= statements(A). {*AST = A;}

statements(A) ::= statements(B) statement(C).   {A = new CompoundNode(B, C); }
statements(A) ::= statement(B).                 {A = B;}

statement(A) ::= assignment(B).           {A = B;}
statement(A) ::= conditional(B).          {A = B;}
statement(A) ::= expression(B) SEMICOLON. {A = B;}

assignment(A) ::= identifier(B) ASSIGN expression(C) SEMICOLON. {
	A = new Node("=", B, C);
}

conditional(A) ::= IF expression(B) block(C). {
	A = new Node("__conditional", B, C);
}

block(A) ::= LBRACE statements(B) RBRACE. {
	A = B;
}

expression(A) ::= expression(B) ADD expression(C). {
	A = new Node("+", B, C);
}
expression(A) ::= expression(B) SUB expression(C). {
	A = new Node("-", B, C);
}
expression(A) ::= expression(B) MULT expression(C). {
	A = new Node("*", B, C);
}
expression(A) ::= expression(B) DIV expression(C). {
	A = new Node("/", B, C);
}
expression(A) ::= expression(B) MOD expression(C). {
	A = new Node("%", B, C);
}
expression(A) ::= expression(B) EQUALS expression(C). {
	A = new Node("==", B, C);
}
expression(A) ::= literal(B). {
	A = B;
}

literal(A) ::= INTEGER(B). {
	A = new Node(B);
}
literal(A) ::= DOUBLE(B). {
	A = new Node(B);
}
literal(A) ::= identifier(B). {
	A = B;
}

identifier(A) ::= IDENTIFIER(B). {
	A = new Node(B);
}
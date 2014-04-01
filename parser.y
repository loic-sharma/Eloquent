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

%left RETURN.
%left COMMA.
%right ASSIGN.
%right EQUALS.
%left ADD SUB.
%left MULT DIV MOD.

program ::= statements(A). {*AST = A;}

statements(A) ::= statements(B) statement(C).   {A = new CompoundNode(B, C); }
statements(A) ::= statement(B).                 {A = B;}

statement(A) ::= function(B).             {A = B;}
statement(A) ::= assignment(B).           {A = B;}
statement(A) ::= conditional(B).          {A = B;}
statement(A) ::= print(B) SEMICOLON.      {A = B;}
statement(A) ::= return(B) SEMICOLON.     {A = B;}
statement(A) ::= expression(B) SEMICOLON. {A = B;}

function(A) ::= FUNCTION identifier(B) declaration_parameters(C) block(D). {
	A = new Node("__function", new Node("__prototype", B, C), D);
}

assignment(A) ::= identifier(B) ASSIGN expression(C) SEMICOLON. {
	A = new Node("=", B, C);
}

conditional(A) ::= IF expression(B) block(C). {
	A = new Node("__conditional", B, C);
}

block(A) ::= LBRACE statements(B) RBRACE. {
	A = B;
}

block(A) ::= LBRACE RBRACE. {
	A = nullptr;
}


print(A) ::= PRINT expression(B). {
	A = new Node("__print", B);
}

return(A) ::= RETURN expression(B). {
	A = new Node("__return", B);	
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
expression(A) ::= function_call(B). {
	A = B;
}
expression(A) ::= literal(B). {
	A = B;
}

function_call(A) ::= identifier(B) call_parameters(C). {
	A = new Node("__call", B, C);
}

declaration_parameters(A) ::= LPAREN declaration_parameter_list(B) RPAREN. {
	A = B;
}
declaration_parameters(A) ::= LPAREN RPAREN. {
	A = nullptr;
}

declaration_parameter_list(A) ::= declaration_parameter_list(B) COMMA identifier(C). {
	A = new Node(",", B, C);
}
declaration_parameter_list(A) ::= identifier(B). {
	A = B;
}

call_parameters(A) ::= LPAREN call_parameter_list(B) RPAREN. {
	A = B;
}
call_parameters(A) ::= LPAREN RPAREN. {
	A = nullptr;
}

call_parameter_list(A) ::= call_parameter_list(B) COMMA expression(C). {
	A = new Node(",", B, C);
}
call_parameter_list(A) ::= expression(B). {
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
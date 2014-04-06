%include
{
	#include <iostream>
	#include <cassert>
	#include <stdlib.h>
	#include "ast.h"
	#include "value.h"
}

%syntax_error
{
	std::cout << "ERROR!\n";
}

%extra_argument {Node **AST}

%token_prefix T_
%token_type {const char *}
%default_type {Node *}

%left RETURN.
%left COMMA.
%right ASSIGN.
%right EQUALS.
%left ADD SUB.
%left MULT DIV MOD.
%left LPAREN RPAREN.

program ::= statements(A). {*AST = A;}

statements(A) ::= statements(B) statement(C).   {A = new Node(Node::Compound, B, C); }
statements(A) ::= statement(B).                 {A = B;}

statement(A) ::= while(B).                {A = B;}
statement(A) ::= function(B).             {A = B;}
statement(A) ::= assignment(B).           {A = B;}
statement(A) ::= conditional(B).          {A = B;}
statement(A) ::= print(B) SEMICOLON.      {A = B;}
statement(A) ::= return(B) SEMICOLON.     {A = B;}
statement(A) ::= expression(B) SEMICOLON. {A = B;}

while(A) ::= WHILE expression(B) block(C). {
	A = new Node(Node::While, B, C);
}

function(A) ::= FUNCTION identifier(B) declaration_parameters(C) block(D). {
	A = new Node(Node::Function, new Node(Node::Prototype, B, C), D);
}

assignment(A) ::= identifier(B) ASSIGN expression(C) SEMICOLON. {
	A = new Node(Node::Assign, B, C);
}

conditional(A) ::= IF expression(B) block(C). {
	Node *branch = new Node(Node::Branch, B, C);

	A = new Node(Node::Conditional, branch, nullptr);
}
conditional(A) ::= IF expression(B) block(C) else(d). {
	Node *branch = new Node(Node::Branch, B, C);

	A = new Node(Node::Conditional, branch, d);
}

else(A) ::= ELSE block(B). {
	A = B;
}

block(A) ::= LBRACE statements(B) RBRACE. {
	A = B;
}

block(A) ::= LBRACE RBRACE. {
	A = nullptr;
}


print(A) ::= PRINT expression(B). {
	A = new Node(Node::Print, B);
}

return(A) ::= RETURN expression(B). {
	A = new Node(Node::Return, B);	
} 

expression(A) ::= LPAREN expression(B) RPAREN. {
	A = B;
}
expression(A) ::= expression(B) ADD expression(C). {
	A = new Node(Node::Add, B, C);
}
expression(A) ::= expression(B) SUB expression(C). {
	A = new Node(Node::Sub, B, C);
}
expression(A) ::= expression(B) MULT expression(C). {
	A = new Node(Node::Mult, B, C);
}
expression(A) ::= expression(B) DIV expression(C). {
	A = new Node(Node::Div, B, C);
}
expression(A) ::= expression(B) MOD expression(C). {
	A = new Node(Node::Mod, B, C);
}
expression(A) ::= expression(B) EQUALS expression(C). {
	A = new Node(Node::Equals, B, C);
}
expression(A) ::= function_call(B). {
	A = B;
}
expression(A) ::= literal(B). {
	A = B;
}

function_call(A) ::= identifier(B) call_parameters(C). {
	A = new Node(Node::Call, B, C);
}

declaration_parameters(A) ::= LPAREN declaration_parameter_list(B) RPAREN. {
	A = B;
}
declaration_parameters(A) ::= LPAREN RPAREN. {
	A = nullptr;
}

declaration_parameter_list(A) ::= declaration_parameter_list(B) COMMA identifier(C). {
	A = new Node(Node::Separator, B, C);
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
	A = new Node(Node::Separator, B, C);
}
call_parameter_list(A) ::= expression(B). {
	A = B;
}

literal(A) ::= INTEGER(B). {
	int value = atoi(B);

	A = new Node(Node::Integer, new Value(value));
}
literal(A) ::= DOUBLE(B). {
	double value = atof(B);

	A = new Node(Node::Double, new Value(value));
}
literal(A) ::= STRING(B). {
	A = new Node(Node::String, new Value(B));
}
literal(A) ::= boolean(B). {
	A = B;
}
literal(A) ::= identifier(B). {
	A = B;
}

boolean(A) ::= TRUE. {
	A = new Node(Node::Boolean, new Value(true));
}
boolean(A) ::= FALSE. {
	A = new Node(Node::Boolean, new Value(false));
}

identifier(A) ::= IDENTIFIER(B). {
	A = new Node(Node::Identifier, new Value(B));
}
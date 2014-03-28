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

%token_type {const char *}
%default_type {const char *}
%extra_argument {AST **}

%right EQUALS.
%left ADD SUB.
%left MULT DIV MOD.

program ::= statements.
statements ::= statements statement.
statements ::= statement.

statement ::= assignment.
statement ::= expression SEMICOLON.

assignment ::= IDENTIFIER(B) EQUALS expression(C) SEMICOLON. {std::cout << B << " = " << C << std::endl;}

expression(A) ::= expression(B) ADD expression(C). {std::cout << B << "+" << C << std::endl; A = std::to_string(std::atoi(B) + std::atoi(C)).c_str();}
expression(A) ::= expression(B) SUB expression(C). {std::cout << B << "-" << C << std::endl; A = std::to_string(std::atoi(B) - std::atoi(C)).c_str();}
expression(A) ::= expression(B) MULT expression(C). {std::cout << B << "*" << C << std::endl;A = std::to_string(std::atoi(B) * std::atoi(C)).c_str();}
expression(A) ::= expression(B) DIV expression(C). {std::cout << B << "/" << C << std::endl;A = std::to_string(std::atoi(B) / std::atoi(C)).c_str();}
expression(A) ::= expression(B) MOD expression(C). {std::cout << B << "%" << C << std::endl;A = std::to_string(std::atoi(B) % std::atoi(C)).c_str();}
expression(A) ::= literal(B). {A = B;}

literal(A) ::= INTEGER(B). {std::cout << "Integer: " << B << std::endl; A = B;}
literal(A) ::= DOUBLE(B). {std::cout << "Double: " << B << std::endl; A = B;}
literal(A) ::= IDENTIFIER(B). {std::cout << "Identifier: " << B << std::endl; A = B;}
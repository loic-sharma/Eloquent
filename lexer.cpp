//
//  lexer.cpp
//  Compiler
//
//  Created by Loic Sharma on 2/19/14.
//  Copyright (c) 2014 sharml. All rights reserved.
//

#include "lexer.h"
#include "parser.h"
#include <iostream>

typedef struct _LexerSpecial {
    const char *value;
    const int type;
} LexerSpecial;

Lexer::Lexer(std::string i)
    : input(i), start(input.c_str()), character(start), line(1) {
}

void Lexer::lex() {
    // We have not peeked at the next token so we will need to manually
    // lex it. Let's get to it!
    skip_whitespace();
    
    // Are we at the last character?
    if (character == nullptr or *character == '\0') {
        token.type = T_EOF;
        token.value = "";
        token.line = line;
    }
    
    else {
        token.type = T_UNKNOWN;
        
        // Lex numbers (integers as well as doubles).
        if (is_number_start()) {
            lex_number();
        }
        else if (is_string_start()) {
            lex_string();
        }
        else {
            // Attempt to lex reserved words and keywords. If the current
            // input token is not special, lex_special will return false.
            if (lex_special() == false) {
                // At this point the token is either an identifier or an unknown
                // token.
                if (is_identifier(*character)) {
                    lex_identifier();
                }
                else {
                    token.value = *character;
                }
            }
        }
    }
}

void Lexer::skip_whitespace() {
    if (character == nullptr) return;

    while (*character == ' ' or *character == '\n' or *character == '\t') {
        if (*character == '\n') {
            ++line;
        }

        ++character;
    }
}

bool Lexer::is_number_start() const {
    const char *current = character;
    
    if (*current == '+' or *current == '-') {
        current++;
    }
    
    return (*current >= '0' and *current <= '9');
}

bool Lexer::is_string_start() const {
    return (*character == '"' or *character == '\'');
}

bool Lexer::is_string_end(char current, char previous, char opening) const {
    if (current == opening) {
        return (previous != '\\');
    }
    
    return false;
}

bool Lexer::is_identifier(char c) const {
    return (c and ((c >= 'a' and c <= 'z') or
                   (c >= 'A' and c <= 'Z') or
                   c == '_'));
}

void Lexer::lex_number() {
    /*
    int sign = 1;
    long integer = 0;

    if (*character == '+' or *character == '-') {
        if (*character == '-') {
            sign = -1;
        }

        ++character;
    }
    
    while (*character >= '0' and *character <= '9') {
        integer = integer * 10 + *character++ - '0';
    }
    */
    
    const char *end = character;

    if (*end == '+' or *end == '-') {
        ++end;
    }

    while (*end >= '0' and *end <= '9') {
        ++end;
    }

    token.type = T_INTEGER;
    token.value = std::string(character, end);
    token.line = line;

    character = end;
}

void Lexer::lex_float() {
    token.type = T_DOUBLE;
    token.value = "0.0";
    token.line = line;
}

void Lexer::lex_string() {
    // The string starts on the next character as the current character is
    // the opening quote.
    const char *string_start = character + 1;
    const char *string_end = string_start;
    
    char opening_char = *character;
    char previous_char = *character;
    
    while (is_string_end(*string_end, previous_char, opening_char) == false) {
        ++string_end;
    }

    // The current character is now the character after the quote that terminates
    // the string that was just lexed.
    character = string_end + 1;
    
    token.type = T_STRING;
    token.value = input.substr(string_start - start, string_end - string_start);
    token.line = line;
}

bool Lexer::lex_special() {
    static const LexerSpecial operators[] = {
        {"+", T_ADD},
        {"-", T_SUB},
        {"*", T_MULT},
        {"/", T_DIV},
        {"%", T_MOD},
        {"=", T_ASSIGN},
        {";", T_SEMICOLON},
        {"{", T_LBRACE},
        {"}", T_RBRACE},
        {"(", T_LPAREN},
        {")", T_RPAREN},
        
        {"if", T_IF},
        {"print", T_PRINT},
        {"function", T_FUNCTION},
        {"return", T_RETURN},/*
        {"elseif", T_ELSEIF},
        {"else", T_ELSE},
        {"and", T_AND},
        {"or", T_OR},
        {"do", T_DO},
        {"while", T_WHILE},
        {"for", T_FOR},
        {"break", T_BREAK},
        {"continue", T_CONTINUE},*/
    };
    
    const char *current = character;
    
    for (LexerSpecial op : operators) {
        const char *value = op.value;
        bool same = true;
        
        while (*value) {
            if (*current == '\0' or *value++ != *current++) {
                same = false;
                break;
            }
        }
        
        if (same == true) {
            character = current;
            
            token.type = op.type;
            token.value = op.value;
            token.line = line;
            
            return true;
        }
        else {
            current = character;
        }
    }
    
    return false;
}

void Lexer::lex_identifier() {
    const char *ident_start = character;
    const char *ident_end = character;
    
    while (is_identifier(*ident_end)) {
        ++ident_end;
    }
    
    character = ident_end;
    
    token.type = T_IDENTIFIER;
    token.value = input.substr(ident_start - start, ident_end - ident_start);
    token.line = line;
}
//
//  lexer.h
//  Compiler
//
//  Created by Loic Sharma on 2/19/14.
//  Copyright (c) 2014 sharml. All rights reserved.
//

#ifndef __Compiler__Lexer__
#define __Compiler__Lexer__

#define T_UNKNOWN -1
#define T_EOF 0

#include <string>

typedef struct {
    int type;
    std::string value;
    int line;
} LexerToken;

class Lexer {

public:
    Lexer(std::string i);

    void lex();
    
    LexerToken token;
    
private:
    // The input string the Lexer is lexing.
    std::string input;
    
    const char *start;
    const char *character;
    
    // The current position's line number of the input.
    unsigned int line;
    
    void skip_skippable();
    void skip_whitespace();
    void skip_comments();

    bool is_comment() const;
    bool is_whitespace() const;
    bool is_number_start() const;
    bool is_string_start() const;
    bool is_string_end(char current, char previous, char opening) const;
    bool is_identifier(char c) const;
    
    void lex_number();
    void lex_float();
    void lex_string();
    bool lex_special();
    void lex_identifier();
};

#endif /* defined(__Compiler__Lexer__) */

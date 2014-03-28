#include <cstdlib>
#include <iostream>
#include <string>
#include "parser.h"
#include "lexer.yy.h"
using namespace std;
 
void* ParseAlloc(void* (*allocProc)(size_t));
void Parse(void* parser, int token, const char *tokenInfo);
void ParseFree(void* parser, void(*freeProc)(void*));

void manual() {
    void *parser = ParseAlloc(malloc);

    Parse(parser, INTEGER, "15");
    Parse(parser, SEMICOLON, ";");
    Parse(parser, 0, 0);

    std::cout << "---------------------\n";

    Parse(parser, INTEGER, "15");
    Parse(parser, ADD, "+");
    Parse(parser, INTEGER, "20");
    Parse(parser, MULT, "*");
    Parse(parser, INTEGER, "5");
    Parse(parser, SEMICOLON, ";");
    Parse(parser, 0, 0);

    std::cout << "---------------------\n";

    Parse(parser, IDENTIFIER, "a");
    Parse(parser, EQUALS, "=");
    Parse(parser, INTEGER, "15");
    Parse(parser, ADD, "+");
    Parse(parser, INTEGER, "20");
    Parse(parser, MULT, "*");
    Parse(parser, INTEGER, "5");
    Parse(parser, SEMICOLON, ";");
    Parse(parser, 0, 0);

    std::cout << "---------------------\n";

    Parse(parser, IDENTIFIER, "a");
    Parse(parser, EQUALS, "=");
    Parse(parser, INTEGER, "15");
    Parse(parser, SEMICOLON, ";");

    Parse(parser, IDENTIFIER, "b");
    Parse(parser, EQUALS, "=");
    Parse(parser, INTEGER, "30");
    Parse(parser, SEMICOLON, ";");
    
    Parse(parser, IDENTIFIER, "c");
    Parse(parser, EQUALS, "=");
    Parse(parser, IDENTIFIER, "a");
    Parse(parser, MULT, "*");
    Parse(parser, IDENTIFIER, "b");
    Parse(parser, SEMICOLON, ";");
    Parse(parser, 0, 0);

    ParseFree(parser, free);
}

/*
void parse(const string& commandLine) {
    // Set up the scanner
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE bufferState = yy_scan_string(commandLine.c_str(), scanner);
 
    // Set up the parser
    void *shellParser = ParseAlloc(malloc);
 
    int lexCode;
    bool validParse = true;
    do {
        lexCode = yylex(scanner);
        std::cout << "Lexed: " << yyget_text(scanner) << std::endl;
        Parse(shellParser, lexCode, yyget_text(scanner), &validParse);
    }
    while (lexCode > 0 && validParse);
 
    if (-1 == lexCode) {
        cerr << "The scanner encountered an error.\n";
    }

    if (!validParse) {
        cerr << "The parser encountered an error.\n";
    }
 
    // Cleanup the scanner and parser
    yy_delete_buffer(bufferState, scanner);
    yylex_destroy(scanner);
    ParseFree(shellParser, free);
}
*/
 
int main() {
    manual();

    return 0;
    /*
    string commandLine;
    while (getline(cin, commandLine)) {
        parse(commandLine);
    }
    return 0;
    */
}
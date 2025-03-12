%option noyywrap
%option yylineno

%{
#include<bits/stdc++.h>
#include "2005006.cpp"
#include "y.tab.h"
using namespace std;
int yycolumn = 1;
#define YY_USER_ACTION \
  yylloc.first_line = yylloc.last_line = yylineno; \
  yylloc.first_column = yycolumn; \
  yylloc.last_column = yycolumn + yyleng - 1; \
  yycolumn += yyleng;

int lineCount = 1;
int errorCount = 0;
int warningCount = 0;


extern ofstream logOut;
extern ofstream errorOut;


int nBuckets = 11;
SymbolTable* symbolTable = new SymbolTable(nBuckets);

string toUpper(string s){
    for(auto &c : s){
        c = toupper(c);
    }
    return s;
}

%}

DIGIT [0-9]
LETTER [a-zA-Z_]
WHITESPACE [ \t]

%%
[\r]?[\n] {
    lineCount++;
    yycolumn = 1;
}


{WHITESPACE} {}


"if" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return IF;
}

"else" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return ELSE;
}

"for" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return FOR;
}

"while" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return WHILE;
}

"int" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return INT;
}

"float" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return FLOAT;
}

"void" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return VOID;
}

"return" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return RETURN;
}

"println" {
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "KEYWORD");
    return PRINTLN;
}

{DIGIT}+ {
    logOut<<"Line# "<<lineCount<<": Token <CONST_INT> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "CONST_INT", "INT");
    return CONST_INT;
}

{DIGIT}*(\.{DIGIT}+) |
{DIGIT}+(\.{DIGIT}*) | 
{DIGIT}+(\.{DIGIT}*)?([Ee][+-]?{DIGIT}+) | 
{DIGIT}*(\.{DIGIT}+)([Ee][+-]?{DIGIT}+) {
    logOut<<"Line# "<<lineCount<<": Token <CONST_FLOAT> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "CONST_FLOAT", "FLOAT");
    return CONST_FLOAT;
}

\+|\- {
    logOut<<"Line# "<<lineCount<<": Token <ADDOP> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "ADDOP");
    return ADDOP;
}

\*|\/|\% {
    logOut<<"Line# "<<lineCount<<": Token <MULOP> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "MULOP");
    return MULOP;
}

\+\+ {
    logOut<<"Line# "<<lineCount<<": Token <INCOP> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "INCOP");
    return INCOP;
}

\-\- {
    logOut<<"Line# "<<lineCount<<": Token <DECOP> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "DECOP");
    return DECOP;
}

\<|\<\=|\>|\>\=|\=\=|\!\= {
    logOut<<"Line# "<<lineCount<<": Token <RELOP> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "RELOP");
    return RELOP;
}

\= {
    logOut<<"Line# "<<lineCount<<": Token <ASSIGNOP> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "ASSIGNOP");
    return ASSIGNOP;
}

\&\&|\|\| {
    logOut<<"Line# "<<lineCount<<": Token <LOGICOP> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "LOGICOP");
    return LOGICOP;
}

"!" {
    logOut<<"Line# "<<lineCount<<": Token <NOT> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "NOT");
    return NOT;
}

"(" {
    logOut<<"Line# "<<lineCount<<": Token <LPAREN> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "LPAREN");
    return LPAREN;
}

")" {
    logOut<<"Line# "<<lineCount<<": Token <RPAREN> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "RPAREN");
    return RPAREN;
}

"{" {
    logOut<<"Line# "<<lineCount<<": Token <LCURL> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "LCURL");
    return LCURL;
}

"}" {
    logOut<<"Line# "<<lineCount<<": Token <RCURL> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "RCURL");
    return RCURL;
}

"[" {
    logOut<<"Line# "<<lineCount<<": Token <LSQUARE> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "LSQUARE");
    return LSQUARE;
}

"]" {
    logOut<<"Line# "<<lineCount<<": Token <RSQUARE> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "RSQUARE");
    return RSQUARE;
}

"," {
    logOut<<"Line# "<<lineCount<<": Token <COMMA> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "COMMA");
    return COMMA;
}

";" {
    logOut<<"Line# "<<lineCount<<": Token <SEMICOLON> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "SEMICOLON");
    return SEMICOLON;
}

{LETTER}({LETTER}|{DIGIT})* {
    logOut<<"Line# "<<lineCount<<": Token <ID> Lexeme "<<yytext<<" found\n";
    yylval.symbolInfo = new SymbolInfo(yytext, "ID");
    return ID;
}


%%


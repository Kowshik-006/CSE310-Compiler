%option noyywrap

%x STRING_STATE
%x SINGLE_LINE_COMMENT
%x MULTI_LINE_COMMENT

%{
#include<bits/stdc++.h>
#include "2005006.cpp"
using namespace std;

int lineCount = 1;
int errorCount = 0;
int warningCount = 0;


ofstream logOut;
ofstream tokenOut;

int nBuckets = 10;
SymbolTable* symbolTable = new SymbolTable(nBuckets);

string character;
int characterCount;

int stringLineCount;
string logString;
string tokenString;

int commentLineCount;
string logComment;

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
[\r]?\n {
    lineCount++;
}

{WHITESPACE} {}

if|else|for|while|do|break|int|char|float|double|void|return|switch|case|default|continue {
    tokenOut<<"<"<<toUpper(yytext)<<", "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <"<<toUpper(yytext)<<"> Lexeme "<<yytext<<" found\n";
}

{DIGIT}+ {
    tokenOut<<"<CONST_INT, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <CONST_INT> Lexeme "<<yytext<<" found\n";
}

{DIGIT}*(\.{DIGIT}+) |
{DIGIT}+(\.{DIGIT}*) | 
{DIGIT}+(\.{DIGIT}*)?([Ee][+-]?{DIGIT}+) | 
{DIGIT}*(\.{DIGIT}+)([Ee][+-]?{DIGIT}+) {
    tokenOut<<"<CONST_FLOAT, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <CONST_FLOAT> Lexeme "<<yytext<<" found\n";
}

{DIGIT}*(\.{DIGIT}*)?([Ee][+-]?{DIGIT}*)+ |
{DIGIT}+(\.{DIGIT}*)?([Ee][+-]?{DIGIT}+)(\.{DIGIT}*)+ | 
{DIGIT}*(\.{DIGIT}+)([Ee][+-]?{DIGIT}+)(\.{DIGIT}*)+  {
    errorCount++;
    logOut<<"Error at line# "<<lineCount<<": ILLFORMED_NUMBER "<<yytext<<endl;
}


{DIGIT}+(\.{DIGIT}*([Ee][+-]?{DIGIT}+)?)* |
{DIGIT}*(\.{DIGIT}+([Ee][+-]?{DIGIT}+)?)+ {
    errorCount++;
    logOut<<"Error at line# "<<lineCount<<": TOO_MANY_DECIMAL_POINTS "<<yytext<<endl;
}

\'\\[(\\)(\')(\")ntafrbv0]\' |
\'\\\' {
    string text = yytext;
    text.erase(0,1);
    text.erase(text.size()-1,1);

    if(text == "\\\\"){
        tokenOut<<"<CONST_CHAR, "<<"\\"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\\"<<" found\n";
    }
    else if(text == "\\\'"){
        tokenOut<<"<CONST_CHAR, "<<"\'"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\'"<<" found\n";
    }
    else if(text == "\\\""){
        tokenOut<<"<CONST_CHAR, "<<"\""<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\""<<" found\n";
    }
    else if(text == "\\n"){
        tokenOut<<"<CONST_CHAR, "<<"\n"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\n"<<" found\n";
    }
    else if(text == "\\t"){
        tokenOut<<"<CONST_CHAR, "<<"\t"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\t"<<" found\n";
    }
    else if(text == "\\a"){
        tokenOut<<"<CONST_CHAR, "<<"\a"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\a"<<" found\n";
    }
    else if(text == "\\f"){
        tokenOut<<"<CONST_CHAR, "<<"\f"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\f"<<" found\n";
    }
    else if(text == "\\r"){
        tokenOut<<"<CONST_CHAR, "<<"\r"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\r"<<" found\n";
    }
    else if(text == "\\b"){
        tokenOut<<"<CONST_CHAR, "<<"\b"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\b"<<" found\n";
    }
    else if(text == "\\v"){
        tokenOut<<"<CONST_CHAR, "<<"\v"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\v"<<" found\n";
    }
    else if(text == "\\0"){
        tokenOut<<"<CONST_CHAR, "<<"\0"<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<"\0"<<" found\n";
    } 
    else{
        errorCount++;
        logOut<<"Error at line# "<<lineCount<<": UNFINISHED_CONST_CHAR "<<yytext<<endl;
    }
}

\'.\' {
    string text = yytext;
    text.erase(0,1);
    text.erase(text.size()-1,1);
    tokenOut<<"<CONST_CHAR, "<<text<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <CONST_CHAR> Lexeme "<<text<<" found\n";
}

\'\' {
    errorCount++;
    logOut<<"Error at line# "<<lineCount<<": EMPTY_CONST_CHAR "<<yytext<<endl;
}

\'\\n {
    errorCount++;
    logOut<<"Error at line# "<<lineCount<<": UNFINISHED_CONST_CHAR "<<yytext<<endl;
}

\'(.)+\' {
    errorCount++;
    logOut<<"Error at line# "<<lineCount<<": MULTICHAR_CONST_CHAR "<<yytext<<endl;
}

\'(.) {
    errorCount++;
    logOut<<"Error at line# "<<lineCount<<": UNFINISHED_CONST_CHAR "<<yytext<<endl;
}


\+|\- {
    tokenOut<<"<ADDOP, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <ADDOP> Lexeme "<<yytext<<" found\n";
}

\*|\/|\% {
    tokenOut<<"<MULOP, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <MULOP> Lexeme "<<yytext<<" found\n";
}

\+\+|\-\- {
    tokenOut<<"<INCOP, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <INCOP> Lexeme "<<yytext<<" found\n";
}

\<|\<\=|\>|\>\=|\=\=|\!\= {
    tokenOut<<"<RELOP, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <RELOP> Lexeme "<<yytext<<" found\n";
}

\= {
    tokenOut<<"<ASSIGNOP, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <ASSIGNOP> Lexeme "<<yytext<<" found\n";
}

\&\&|\|\| {
    tokenOut<<"<LOGICOP, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <LOGICOP> Lexeme "<<yytext<<" found\n";
}

\&|\||\^|\<\<|\>\> {
    tokenOut<<"<BITOP, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <BITOP> Lexeme "<<yytext<<" found\n";
}



"!" {
    tokenOut<<"<NOT, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <NOT> Lexeme "<<yytext<<" found\n";
}

"(" {
    tokenOut<<"<LPAREN, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <LPAREN> Lexeme "<<yytext<<" found\n";
}

")" {
    tokenOut<<"<RPAREN, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <RPAREN> Lexeme "<<yytext<<" found\n";
}

"{" {
    tokenOut<<"<LCURL, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <LCURL> Lexeme "<<yytext<<" found\n";
    symbolTable->enterScope();
}

"}" {
    tokenOut<<"<RCURL, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <RCURL> Lexeme "<<yytext<<" found\n";
    symbolTable->exitScope();
}

"[" {
    tokenOut<<"<LSQUARE, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <LSQUARE> Lexeme "<<yytext<<" found\n";
}

"]" {
    tokenOut<<"<RSQUARE, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <RSQUARE> Lexeme "<<yytext<<" found\n";
}

"," {
    tokenOut<<"<COMMA, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <COMMA> Lexeme "<<yytext<<" found\n";
}

";" {
    tokenOut<<"<SEMICOLON, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <SEMICOLON> Lexeme "<<yytext<<" found\n";
}

{LETTER}({LETTER}|{DIGIT})* {
    tokenOut<<"<ID, "<<yytext<<">\n";
    logOut<<"Line# "<<lineCount<<": Token <ID> Lexeme "<<yytext<<" found\n";
    
    //Handle already existing IDs
    bool success = symbolTable->Insert(yytext, "ID");
    if(!success){
        logOut<<"\t"<<yytext<<" already exists in the current ScopeTable\n";
    }
    else{
        symbolTable->printAll(logOut);
    } 
}

{DIGIT}+({LETTER}|{DIGIT})* {
    errorCount++;
    logOut<<"Error at line# "<<lineCount<<": INVALID_ID_SUFFIX_NUM_PREFIX "<<yytext<<endl;
}

\" {
   stringLineCount = 1;
   logString = "\"";
   tokenString = "";
   BEGIN STRING_STATE; 
}

<STRING_STATE>\" {
    logString += "\"";
    if(stringLineCount > 1){
        tokenOut<<"<MULTI LINE STRING, "<<tokenString<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <MULTI LINE STRING> Lexeme "<<logString<<" found\n";
    }
    else{
        tokenOut<<"<SINGLE LINE STRING, "<<tokenString<<">\n";
        logOut<<"Line# "<<lineCount<<": Token <SINGLE LINE STRING> Lexeme "<<logString<<" found\n";
    }
    lineCount += stringLineCount -1;
    BEGIN INITIAL;
}

<STRING_STATE>\\\n {
    logString += yytext; 
    stringLineCount++;
}

<STRING_STATE>\\\r\n {
    logString += yytext; 
    stringLineCount++;
}

<STRING_STATE>[\r]?\n {
    errorCount++;
    lineCount += stringLineCount - 1;
    logOut<<"Error at line# "<<lineCount<<": UNFINISHED_STRING "<<logString<<endl;
    lineCount++;
    BEGIN INITIAL;
}

<STRING_STATE>\\n {
    logString += yytext; 
    tokenString += "\n"; 
}

<STRING_STATE>\\t {
    logString += yytext; 
    tokenString += "\t"; 
}

<STRING_STATE>\\\' {
    logString += yytext; 
    tokenString += "\'"; 
}

<STRING_STATE>\\\" {
    logString += yytext; 
    tokenString += "\""; 
}

<STRING_STATE>\\\\ {
    logString += yytext; 
    tokenString += "\\";
}

<STRING_STATE>\\a {
    logString += yytext; 
    tokenString += "\a"; 
}

<STRING_STATE>\\f {
    logString += yytext; 
    tokenString += "\f"; 
}

<STRING_STATE>\\r {
    logString += yytext; 
    tokenString += "\r"; 
}

<STRING_STATE>\\b {
    logString += yytext; 
    tokenString += "\b"; 
}

<STRING_STATE>\\v {
    logString += yytext; 
    tokenString += "\v"; 
}

<STRING_STATE>\\0 {
    logString += yytext; 
    tokenString += "\0"; 
}

<STRING_STATE><<EOF>> {
    errorCount++;
    lineCount += stringLineCount -1;
    logOut<<"Error at line# "<<lineCount<<": UNFINISHED_STRING "<<logString<<endl;
    BEGIN INITIAL;
}

<STRING_STATE>. {
    logString += yytext[0]; 
    tokenString += yytext[0]; 
}

\/\/ {
    commentLineCount = 1;
    logComment = yytext; // "//"
    BEGIN SINGLE_LINE_COMMENT;
}

<SINGLE_LINE_COMMENT>[\r]?\n {
    logOut<<"Line# "<<lineCount<<": Token <SINGLE LINE COMMENT> Lexeme "<<logComment<<" found\n";
    lineCount += commentLineCount;
    BEGIN INITIAL;
}

<SINGLE_LINE_COMMENT>\\\n {
    commentLineCount++;
    logComment += yytext;
}

<SINGLE_LINE_COMMENT>\\\r\n {
    commentLineCount++;
    logComment += yytext;
}

<SINGLE_LINE_COMMENT>\\(.)+ {
    logComment += yytext;
}

<SINGLE_LINE_COMMENT>. {
    logComment += yytext[0];
}

<SINGLE_LINE_COMMENT><<EOF>> {
    logOut<<"Line# "<<lineCount<<": Token <SINGLE LINE COMMENT> Lexeme "<<logComment<<" found\n";
    lineCount += commentLineCount - 1;
    BEGIN INITIAL;   
}

\/\* {
    commentLineCount = 1;
    logComment = yytext; // "*/"
    BEGIN MULTI_LINE_COMMENT;
      
}

<MULTI_LINE_COMMENT>\*\/ {
    logComment += yytext; // "*/"
    logOut<<"Line# "<<lineCount<<": Token <MULTI LINE COMMENT> Lexeme "<<logComment<<" found\n";
    lineCount += commentLineCount-1;
    BEGIN INITIAL;
}

<MULTI_LINE_COMMENT>\\\n {
    commentLineCount++;
    logComment += yytext;
}

<MULTI_LINE_COMMENT>\\\r\n {
    commentLineCount++;
    logComment += yytext;
}

<MULTI_LINE_COMMENT>\n|\r\n {
    commentLineCount++;
    logComment += yytext;
}

<MULTI_LINE_COMMENT>. {
    logComment += yytext[0];
}

<MULTI_LINE_COMMENT><<EOF>> {
    errorCount++;
    lineCount += commentLineCount - 1;
    logOut<<"Error at line# "<<lineCount<<": UNFINISHED_COMMENT "<<logComment<<endl;
    BEGIN INITIAL;   
}

. {
    errorCount++;
    logOut<<"Error at line# "<<lineCount<<": UNRECOGNIZED_CHAR "<<yytext<<endl;
}



%%
int main(int argc,char *argv[]){

	
	if(argc!=2){
		printf("Please provide input file name and try again\n");
		return 0;
	}
	
	FILE *fin=fopen(argv[1],"r");
	if(fin==NULL){
		printf("Cannot open specified file\n");
		return 0;
	}

    logOut.open("2005006_log.txt");
    tokenOut.open("2005006_token.txt");

	yyin= fin;
	yylex();
	fclose(yyin);

    symbolTable->printAll(logOut);

    logOut<<"Total lines: "<<lineCount<<endl;
    logOut<<"Total errors: "<<errorCount<<endl;
    logOut<<"Total warnings: "<<warningCount<<endl;

    logOut.close();
    tokenOut.close();
	return 0;
}


%{
#include <bits/stdc++.h>
#include"2005006.cpp"
using namespace std;

ofstream logOut;
ofstream errorOut;
ofstream parseTreeOut;

extern SymbolTable* symbolTable;

extern int lineCount;
extern int errorCount;
extern FILE* yyin;
#define YYERROR_CALL(msg) yyerror(msg)

void yyerror(char* s){
   // cout<<s<<endl;
}

// Apatoto bujhtesina
int yyparse(void);
int yylex(void);

vector<SymbolInfo*> params;

%}

/* %error-verbose */

%locations


%union{
    SymbolInfo* symbolInfo;
    Node* node;
    struct {
        int first_line;
        int last_line;
    } location;
}

%type<node> start program unit func_declaration func_definition parameter_list compound_statement var_declaration type_specifier declaration_list statements statement expression_statement variable expression logic_expression rel_expression simple_expression term unary_expression factor argument_list arguments
%token<symbolInfo> ID LPAREN RPAREN SEMICOLON COMMA LCURL RCURL INT FLOAT VOID LSQUARE RSQUARE CONST_INT CONST_FLOAT IF ELSE FOR WHILE PRINTLN RETURN ASSIGNOP LOGICOP RELOP ADDOP MULOP NOT INCOP DECOP 

// Makes Logical and Relational operators non associative
/* %nonassoc LOGICOP RELOP
%nonassoc ELSE */

%nonassoc RPAREN
%nonassoc ELSE

%%

// Grammar Rules

start : program 
    {
        // Creating a new node of the Parse Tree
        $$ = new Node(@$.first_line, @$.last_line,false);
        // Setting the rule of the node
        $$->setRule("start : program");
        // Adding the first symbol on the right side as a child  
        $$->addChildNode($1);

        logOut<<$$->getRule()<<endl;
        $$->printParseTree(parseTreeOut,$$,1);
    }
    ;

program : program unit 
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("program : program unit");
        $$->addChildNode($1);
        $$->addChildNode($2);
        logOut<<$$->getRule()<<endl;
    } 
    | unit 
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("program : unit");
        $$->addChildNode($1);
        logOut<<$$->getRule()<<endl;
    }
    ;

unit : var_declaration 
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("unit : var_declaration");
        $$->addChildNode($1);
        logOut<<$$->getRule()<<endl;
    }
    | func_declaration
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("unit : func_declaration");
        $$->addChildNode($1);
        logOut<<$$->getRule()<<endl;
    }
    | func_definition
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("unit : func_definition");
        $$->addChildNode($1);
        logOut<<$$->getRule()<<endl;
    }
    ;

func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON
    {
        Node* idNode = new Node(@2.first_line, @2.last_line,true);
        idNode->setRule("ID : "+$2->getName());
        Node* lparenNode = new Node(@3.first_line, @3.last_line,true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@5.first_line, @5.last_line,true);
        rparenNode->setRule("RPAREN : )");
        Node* semicolonNode = new Node(@6.first_line, @6.last_line,true);
        semicolonNode->setRule("SEMICOLON : ;");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
        $$->addChildNode($1);
        $$->addChildNode(idNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode($4);
        $$->addChildNode(rparenNode);
        $$->addChildNode(semicolonNode);
        
        
        SymbolInfo* idSymbol = symbolTable->LookupGlobalScope($2->getName());
        if(idSymbol != nullptr){
            if(idSymbol->getType() == "FUNCTION"){
                errorOut<<"Line# "<<@$.first_line<<": Redeclaration of function \'"<<idSymbol->getName()<<"\'\n";
                errorCount++;
            }
            //Change kora lagte pare
            else if(idSymbol->getType() == "VARIABLE"){
                errorOut<<"Line# "<<@$.first_line<<": Redeclaration of \'"<<idSymbol->getName()<<"\' as different kind of symbol\n";
                errorCount++;
            }
        } 
        else{
            idSymbol = new SymbolInfo($2->getName(),"FUNCTION",$1->getType());
            // Inserting the parameters into the function's symbolInfo
            idSymbol->setParamList($4->getParameterList());
            idSymbol->setFuncDeclStatus(true);

            // Inserting the function in the symbol table
            symbolTable->Insert(idSymbol);
        }

        logOut<<$$->getRule()<<endl;
        

    }
    | type_specifier ID LPAREN RPAREN SEMICOLON
    {
        Node* idNode = new Node(@2.first_line, @2.last_line,true);
        idNode->setRule("ID : "+$2->getName());
        Node* lparenNode = new Node(@3.first_line, @3.last_line,true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@4.first_line, @4.last_line,true);
        rparenNode->setRule("RPAREN : )");
        Node* semicolonNode = new Node(@5.first_line, @5.last_line,true);
        semicolonNode->setRule("SEMICOLON : ;");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON");
        $$->addChildNode($1);
        $$->addChildNode(idNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode(rparenNode);
        $$->addChildNode(semicolonNode);

        SymbolInfo* idSymbol = symbolTable->LookupGlobalScope($2->getName());
        if(idSymbol != nullptr){
            if(idSymbol->getType() == "FUNCTION"){
                errorOut<<"Line# "<<@$.first_line<<": Redeclaration of function \'"<<idSymbol->getName()<<"\'\n";
                errorCount++;
            }
            //Change kora lagte pare
            else if(idSymbol->getType() == "VARIABLE"){
                errorOut<<"Line# "<<@$.first_line<<": Redeclaration of \'"<<idSymbol->getName()<<"\' as different kind of symbol\n";
                errorCount++;
            }
        } 
        else{
            idSymbol = new SymbolInfo($2->getName(),"FUNCTION",$1->getType());
            
            idSymbol->setFuncDeclStatus(true);

            // Inserting the function in the symbol table
            symbolTable->Insert(idSymbol);  
        }

        logOut<<$$->getRule()<<endl;
        
    }
    ;

func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement
    {   
        Node* idNode = new Node(@2.first_line, @2.last_line,true);
        idNode->setRule("ID : "+$2->getName());
        Node* lparenNode = new Node(@3.first_line, @3.last_line,true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@5.first_line, @5.last_line,true);
        rparenNode->setRule("RPAREN : )");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement");
        $$->addChildNode($1);
        $$->addChildNode(idNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode($4);
        $$->addChildNode(rparenNode);
        $$->addChildNode($6);


        SymbolInfo* idSymbol = symbolTable->LookupGlobalScope($2->getName());
        if(idSymbol != nullptr){
            if(idSymbol->getType()=="FUNCTION"){
                if(idSymbol->getFuncDefnStatus()){
                    errorOut<<"Line# "<<@$.first_line<<": Redeclaration of function \'"<<idSymbol->getName()<<"\'\n";            
                    errorCount++;
                }
                else if(idSymbol->getFuncDeclStatus()){
                    bool hasErrors = false;
                    if(idSymbol->getDataType() != $1->getType()){
                        errorOut<<"Line# "<<@$.first_line<<": Conflicting types for \'"<<idSymbol->getName()<<"\'\n";
                        errorCount++;
                        hasErrors = true;
                    }
                    if(idSymbol->getParamList().size() != $4->getParameterList().size()){
                        // Change hoite pare
                        errorOut<<"Line# "<<@$.first_line<<": Conflicting types for \'"<<idSymbol->getName()<<"\'\n";
                        errorCount++;
                        hasErrors = true;
                    }
                    // Determining the minimum number of parameters to check
                    int nParameters = (idSymbol->getParamList().size() <= $4->getParameterList().size()) ? idSymbol->getParamList().size() : $4->getParameterList().size();
                    for(int i=0;i<nParameters;i++){
                        if((idSymbol->getParamList()[i]->getDataType() != $4->getParameterList()[i]->getDataType())
                        || (idSymbol->getParamList()[i]->getName() != $4->getParameterList()[i]->getName())){
                            errorOut<<"Line# "<<@$.first_line<<": Conflicting types for \'"<<idSymbol->getName()<<"\'\n";
                            errorCount++;
                            hasErrors = true;
                            break;
                        }
                    }
                    if(!hasErrors){
                        idSymbol->setFuncDefnStatus(true);
                    }
                }

            }
            else if(idSymbol->getType() == "VARIABLE"){
                errorOut<<"Line# "<<@$.first_line<<": Redeclaration of \'"<<$2<<"\' as different kind of symbol\n";
                errorCount++;
            }

        }
        else{
            idSymbol = new SymbolInfo($2->getName(),"FUNCTION",$1->getType());

            // Inserting the parameters into the function's symbolInfo
            idSymbol->setParamList($4->getParameterList());
            idSymbol->setFuncDefnStatus(true);

            // Inserting the function in the symbol table
            symbolTable->Insert(idSymbol);
        }
        logOut<<$$->getRule()<<endl;
    }
    | type_specifier ID LPAREN RPAREN compound_statement
    {
        Node* idNode = new Node(@2.first_line, @2.last_line,true);
        idNode->setRule("ID : "+$2->getName());
        Node* lparenNode = new Node(@3.first_line, @3.last_line,true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@4.first_line, @4.last_line,true);
        rparenNode->setRule("RPAREN : )");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("func_definition : type_specifier ID LPAREN RPAREN compound_statement");
        $$->addChildNode($1);
        $$->addChildNode(idNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode(rparenNode);
        $$->addChildNode($5);

        SymbolInfo* idSymbol = symbolTable->LookupGlobalScope($2->getName());
        if(idSymbol != nullptr){
            if(idSymbol->getType()=="FUNCTION"){
                if(idSymbol->getFuncDefnStatus()){
                    errorOut<<"Line# "<<@$.first_line<<": Redeclaration of function \'"<<idSymbol->getName()<<"\'\n";            
                    errorCount++;
                }
                else if(idSymbol->getFuncDeclStatus()){
                    bool hasErrors = false;
                    if(idSymbol->getDataType() != $1->getType()){
                        errorOut<<"Line# "<<@$.first_line<<": Conflicting types for \'"<<idSymbol->getName()<<"\'\n";
                        errorCount++;
                        hasErrors = true;
                    }
                    if(idSymbol->getParamList().size() != 0){
                        // Change hoite pare
                        errorOut<<"Line# "<<@$.first_line<<": Conflicting types for \'"<<idSymbol->getName()<<"\'\n";
                        errorCount++;
                        hasErrors = true;
                    }
                    if(!hasErrors){
                        idSymbol->setFuncDefnStatus(true);
                    }
                }

            }
            else if(idSymbol->getType() == "VARIABLE"){
                errorOut<<"Line# "<<@$.first_line<<": Redeclaration of \'"<<idSymbol->getName()<<"\' as different kind of symbol\n";
                errorCount++;
            }

        }
        else{
            idSymbol = new SymbolInfo($2->getName(),"FUNCTION",$1->getType());
            idSymbol->setFuncDefnStatus(true);
            symbolTable->Insert(idSymbol);
        }
        logOut<<$$->getRule()<<endl;
    }
    ;

parameter_list : parameter_list COMMA type_specifier ID
    {
        Node* commaNode = new Node(@2.first_line,@2.last_line, true);
        commaNode->setRule("COMMA : ,");
        Node* idNode = new Node(@4.first_line,@4.last_line, true);
        idNode->setRule("ID : "+$4->getName());

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("parameter_list : parameter_list COMMA type_specifier ID");
        $$->addChildNode($1);
        $$->addChildNode(commaNode);
        $$->addChildNode($3);
        $$->addChildNode(idNode);

        bool alreadyExists = false;
        for(SymbolInfo* param : $1->getParameterList()){
            if(param->getName() == $4->getName()){
                alreadyExists = true;
            }
            $$->addParameter(param);
        }

        if(alreadyExists){
            errorOut<<"Line# "<<@$.first_line<<": Redefinition of parameter \'"<<$4->getName()<<"\'\n";
            errorCount++;
        }
        else{
            SymbolInfo* paramSymbol = new SymbolInfo($4->getName(),"PARAMETER",$3->getType());
            $$->addParameter(paramSymbol);
            // symbolTable->Insert(paramSymbol);
        }
        params = $$->getParameterList();
        logOut<<$$->getRule()<<endl;
    }
    | parameter_list COMMA type_specifier
    {
        Node* commaNode = new Node(@2.first_line,@2.last_line, true);
        commaNode->setRule("COMMA : ,");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("parameter_list : parameter_list COMMA type_specifier");
        $$->addChildNode($1);
        $$->addChildNode(commaNode);
        $$->addChildNode($3);

        for(SymbolInfo* param : $1->getParameterList()){
            $$->addParameter(param);
        }
        $$->addParameter(new SymbolInfo("","PARAMETER",$3->getType())); // Kahini ase 
        params = $$->getParameterList();
        logOut<<$$->getRule()<<endl;
    }
    | type_specifier ID
    {
        Node* idNode = new Node(@2.first_line,@2.last_line, true);
        idNode->setRule("ID : "+$2->getName());

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("parameter_list : type_specifier ID");
        $$->addChildNode($1);
        $$->addChildNode(idNode);

        SymbolInfo* paramSymbol = new SymbolInfo($2->getName(),"PARAMETER",$1->getType()); // Kahini ase
        $$->addParameter(paramSymbol);
        // symbolTable->Insert(paramSymbol);
        logOut<<$$->getRule()<<endl;
        params = $$->getParameterList();
    }
    | type_specifier
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("parameter_list : type_specifier");
        $$->addChildNode($1);

        $$->addParameter(new SymbolInfo("","PARAMETER",$1->getType()));
        params = $$->getParameterList();
        logOut<<$$->getRule()<<endl;
    }
    ;

compound_statement : LCURL 
    {
        symbolTable->enterScope();
        
        if(params.size()>0){
            for(SymbolInfo* param : params){
                SymbolInfo* tempParam = new SymbolInfo(param); // So that, exiting the scope doesn't delete the parameters
                symbolTable->Insert(tempParam);
            }
            params.clear();
        }
    }
    statements RCURL
    {
        Node* lcurlNode = new Node(@1.first_line,@1.last_line, true);
        lcurlNode->setRule("LCURL : {");
        Node* rcurlNode = new Node(@4.first_line,@4.last_line, true);
        rcurlNode->setRule("RCURL : }");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("compound_statement : LCURL statements RCURL");
        $$->addChildNode(lcurlNode);
        $$->addChildNode($3);
        $$->addChildNode(rcurlNode);
        
        logOut<<$$->getRule()<<endl;
        
        // Printing the symbol table
        symbolTable->printAll(logOut);

        symbolTable->exitScope();
    }
    | LCURL RCURL
    {
        Node* lcurlNode = new Node(@1.first_line,@1.last_line, true);
        lcurlNode->setRule("LCURL : {");
        Node* rcurlNode = new Node(@2.first_line,@2.last_line, true);
        rcurlNode->setRule("RCURL : }");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("compound_statement : LCURL RCURL");
        $$->addChildNode(lcurlNode);
        $$->addChildNode(rcurlNode);

        logOut<<$$->getRule()<<endl;
        // Printing the symbol table
        symbolTable->printAll(logOut);
    }
    ;

var_declaration : type_specifier declaration_list SEMICOLON
    {
        Node* semicolonNode = new Node(@3.first_line,@3.last_line, true);
        semicolonNode->setRule("SEMICOLON : ;");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("var_declaration : type_specifier declaration_list SEMICOLON");
        $$->addChildNode($1);
        $$->addChildNode($2);
        $$->addChildNode(semicolonNode);

        if($1->getType()== "VOID"){
            for(SymbolInfo* var : $2->getParameterList()){
                errorOut<<"Line# "<<@1.first_line<<": Variable or field \'"<<var->getName()<<"\' declared void\n";
                errorCount++;
            }
        }
        else{
            for(SymbolInfo* var : $2->getParameterList()){
                var->setDataType($1->getType());
            }
        }
        logOut<<$$->getRule()<<endl;
    }
    ;

type_specifier : INT
    {
        Node* intNode = new Node(@1.first_line,@1.last_line, true);
        intNode->setRule("INT : int");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("type_specifier : INT");
        $$->addChildNode(intNode);
        $$->setType("INT");
        logOut<<$$->getRule()<<endl;
    }
    | FLOAT
    {
        Node* floatNode = new Node(@1.first_line,@1.last_line, true);
        floatNode->setRule("FLOAT : float");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("type_specifier : FLOAT");
        $$->addChildNode(floatNode);
        $$->setType("FLOAT");
        logOut<<$$->getRule()<<endl;
    }
    | VOID
    {
        Node* voidNode = new Node(@1.first_line,@1.last_line, true);
        voidNode->setRule("VOID : void");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("type_specifier : VOID");
        $$->addChildNode(voidNode);
        $$->setType("VOID");
        logOut<<$$->getRule()<<endl;
    }
    ;

declaration_list : declaration_list COMMA ID
    {
        Node* commaNode = new Node(@2.first_line,@2.last_line, true);
        commaNode->setRule("COMMA : ,");
        Node* idNode = new Node(@3.first_line,@3.last_line, true);
        idNode->setRule("ID : "+$3->getName());

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("declaration_list : declaration_list COMMA ID");
        $$->addChildNode($1);
        $$->addChildNode(commaNode);
        $$->addChildNode(idNode);

        bool alreadyExists = false;
        for(SymbolInfo* var : $1->getParameterList()){
            if(var->getName() == $3->getName()){
                alreadyExists = true;
            }
            $$->addParameter(var);
        }
        
        if(alreadyExists){
            errorOut<<"Line# "<<@3.first_line<<": Redeclaration of \'"<<$3->getName()<<"\'\n";
            errorCount++;
        }
        else{
            SymbolInfo* idSymbol = new SymbolInfo($3->getName(),"VARIABLE",$1->getType());
            symbolTable->Insert(idSymbol);
            $$->addParameter(idSymbol);
        }
        logOut<<$$->getRule()<<endl;
    }
    | declaration_list COMMA ID LSQUARE CONST_INT RSQUARE
    {
        Node* commaNode = new Node(@2.first_line,@2.last_line, true);
        commaNode->setRule("COMMA : ,");
        Node* idNode = new Node(@3.first_line,@3.last_line, true);
        idNode->setRule("ID : "+$3->getName());
        Node* lthirdNode = new Node(@4.first_line,@4.last_line, true);
        lthirdNode->setRule("LSQUARE : [");
        Node* const_intNode = new Node(@5.first_line,@5.last_line, true);
        const_intNode->setRule("CONST_INT : "+$5->getName());
        Node* rthirdNode = new Node(@6.first_line,@6.last_line, true);
        rthirdNode->setRule("RSQUARE : ]");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("declaration_list : declaration_list COMMA ID LSQUARE CONST_INT RSQUARE");
        $$->addChildNode($1);
        $$->addChildNode(commaNode);
        $$->addChildNode(idNode);
        $$->addChildNode(lthirdNode);
        $$->addChildNode(const_intNode);
        $$->addChildNode(rthirdNode);

        bool alreadyExists = false;
        for(SymbolInfo* var : $1->getParameterList()){
            if(var->getName()==$3->getName()){
                alreadyExists = true;
            }
            $$->addParameter(var);
        }
        if(alreadyExists){
            errorOut<<"Line# "<<@3.first_line<<": Redeclaration of \'"<<$3->getName()<<"\'\n";
            errorCount++;
        }
        else{
            SymbolInfo* idSymbol = new SymbolInfo($3->getName(),"ARRAY",$1->getType());
            idSymbol->setArrayStatus(true);
            idSymbol->setArraySize(stoi($5->getName()));
            symbolTable->Insert(idSymbol);
            $$->addParameter(idSymbol);
        }
        logOut<<$$->getRule()<<endl;
    }
    | ID
    {
        Node* idNode = new Node(@1.first_line,@1.last_line, true);
        idNode->setRule("ID : "+$1->getName());

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("declaration_list : ID");
        $$->addChildNode(idNode);

        SymbolInfo* idSymbol = new SymbolInfo($1->getName(),"VARIABLE",$1->getType());
        bool hasInsterted = symbolTable->Insert(idSymbol);
        if(!hasInsterted){
            errorOut<<"Line# "<<@1.first_line<<": Redeclaration of \'"<<idSymbol->getName()<<"\'\n";
            errorCount++;
        }
        else{
            $$->addParameter(idSymbol);
        }
        logOut<<$$->getRule()<<endl;
    }
    | ID LSQUARE CONST_INT RSQUARE
    {
        Node* idNode = new Node(@1.first_line,@1.last_line, true);
        idNode->setRule("ID : "+$1->getName());
        Node* lthirdNode = new Node(@2.first_line,@2.last_line, true);
        lthirdNode->setRule("LSQUARE : [");
        Node* const_intNode = new Node(@3.first_line,@3.last_line, true);
        const_intNode->setRule("CONST_INT : "+$3->getName());
        Node* rthirdNode = new Node(@4.first_line,@4.last_line, true);
        rthirdNode->setRule("RSQUARE : ]");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("declaration_list : ID LSQUARE CONST_INT RSQUARE");
        $$->addChildNode(idNode);
        $$->addChildNode(lthirdNode);
        $$->addChildNode(const_intNode);
        $$->addChildNode(rthirdNode);

        SymbolInfo* idSymbol = new SymbolInfo($1->getName(),"ARRAY",$1->getType());
        idSymbol->setArrayStatus(true);
        idSymbol->setArraySize(stoi($3->getName()));
        bool hasInsterted = symbolTable->Insert(idSymbol);
        if(!hasInsterted){
            errorOut<<"Line# "<<@1.first_line<<": Redeclaration of \'"<<idSymbol->getName()<<"\'\n";
            errorCount++;
        }
        else{
            $$->addParameter(idSymbol);
        }
        logOut<<$$->getRule()<<endl;
    }
    | ID LSQUARE RSQUARE ASSIGNOP LCURL arguments RCURL
    {
        Node* idNode = new Node(@1.first_line,@1.last_line, true);
        idNode->setRule("ID : "+$1->getName());
        Node* lthirdNode = new Node(@2.first_line,@2.last_line, true);
        lthirdNode->setRule("LSQUARE : [");
        Node* rthirdNode = new Node(@3.first_line,@3.last_line, true);
        rthirdNode->setRule("RSQUARE : ]");
        Node* assignopNode = new Node(@4.first_line,@4.last_line,true);
        assignopNode->setRule("ASSIGNOP : =");
        Node* lcurlNode = new Node(@5.first_line,@5.last_line, true);
        lcurlNode->setRule("LCURL : {");
        Node* rcurlNode = new Node(@7.first_line,@7.last_line, true);
        rcurlNode->setRule("RCURL : }");

        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("declaration_list : ID LSQUARE RSQUARE ASSIGNOP LCURL arguments RCURL");
        $$->addChildNode(idNode);
        $$->addChildNode(lthirdNode);
        $$->addChildNode(rthirdNode);
        $$->addChildNode(assignopNode);
        $$->addChildNode(lcurlNode);
        $$->addChildNode($6);
        $$->addChildNode(rcurlNode);
        logOut<<$$->getRule()<<endl;
    }
    ;

statements : statement 
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statements : statement");
        $$->addChildNode($1);
        logOut<<$$->getRule()<<endl;
    }
    | statements statement 
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statements : statements statement");
        $$->addChildNode($1);
        $$->addChildNode($2);
        logOut<<$$->getRule()<<endl;
    }
    ;

statement : var_declaration
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : var_declaration");
        $$->addChildNode($1);
        logOut<<$$->getRule()<<endl;
    }
    | expression_statement
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : expression_statement");
        $$->addChildNode($1);
        logOut<<$$->getRule()<<endl;
    }
    | compound_statement
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : compound_statement");
        $$->addChildNode($1);
        logOut<<$$->getRule()<<endl;
    }
    | FOR LPAREN expression_statement expression_statement expression RPAREN statement
    {
        Node* forNode = new Node(@1.first_line,@1.last_line, true);
        forNode->setRule("FOR : for");
        Node* lparenNode = new Node(@2.first_line,@2.last_line, true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@6.first_line,@6.last_line, true);
        rparenNode->setRule("RPAREN : )");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement");
        $$->addChildNode(forNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode($3);
        $$->addChildNode($4);
        $$->addChildNode($5);
        $$->addChildNode(rparenNode);
        $$->addChildNode($7);

        logOut<<$$->getRule()<<endl;
    }
    | IF LPAREN expression RPAREN statement
    {
        Node* ifNode = new Node(@1.first_line,@1.last_line, true);
        ifNode->setRule("IF : if");
        Node* lparenNode = new Node(@2.first_line,@2.last_line, true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@4.first_line,@4.last_line, true);
        rparenNode->setRule("RPAREN : )");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : IF LPAREN expression RPAREN statement");
        $$->addChildNode(ifNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode($3);
        $$->addChildNode(rparenNode);
        $$->addChildNode($5);

        logOut<<$$->getRule()<<endl;
    }
    | IF LPAREN expression RPAREN statement ELSE statement
    {
        Node* ifNode = new Node(@1.first_line,@1.last_line, true);
        ifNode->setRule("IF : if");
        Node* lparenNode = new Node(@2.first_line,@2.last_line, true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@4.first_line,@4.last_line, true);
        rparenNode->setRule("RPAREN : )");
        Node* elseNode = new Node(@6.first_line,@6.last_line, true);
        elseNode->setRule("ELSE : else");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : IF LPAREN expression RPAREN statement ELSE statement");
        $$->addChildNode(ifNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode($3);
        $$->addChildNode(rparenNode);
        $$->addChildNode($5);
        $$->addChildNode(elseNode);
        $$->addChildNode($7);

        logOut<<$$->getRule()<<endl;
    }
    | WHILE LPAREN expression RPAREN statement
    {
        Node* whileNode = new Node(@1.first_line,@1.last_line, true);
        whileNode->setRule("WHILE : while");
        Node* lparenNode = new Node(@2.first_line,@2.last_line, true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@4.first_line,@4.last_line, true);
        rparenNode->setRule("RPAREN : )");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : WHILE LPAREN expression RPAREN statement");
        $$->addChildNode(whileNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode($3);
        $$->addChildNode(rparenNode);
        $$->addChildNode($5);

        logOut<<$$->getRule()<<endl;
    }
    | PRINTLN LPAREN expression RPAREN SEMICOLON
    {
        Node* printlnNode = new Node(@1.first_line,@1.last_line, true);
        printlnNode->setRule("PRINTLN : println");
        Node* lparenNode = new Node(@2.first_line,@2.last_line, true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@4.first_line,@4.last_line, true);
        rparenNode->setRule("RPAREN : )");
        Node* semicolonNode = new Node(@5.first_line,@5.last_line, true);
        semicolonNode->setRule("SEMICOLON : ;");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : PRINTLN LPAREN expression RPAREN SEMICOLON");
        $$->addChildNode(printlnNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode($3);
        $$->addChildNode(rparenNode);
        $$->addChildNode(semicolonNode);

        logOut<<$$->getRule()<<endl;
    }
    | RETURN expression SEMICOLON
    {
        Node* returnNode = new Node(@1.first_line,@1.last_line, true);
        returnNode->setRule("RETURN : return");
        Node* semicolonNode = new Node(@3.first_line,@3.last_line, true);
        semicolonNode->setRule("SEMICOLON : ;");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("statement : RETURN expression SEMICOLON");
        $$->addChildNode(returnNode);
        $$->addChildNode($2);
        $$->addChildNode(semicolonNode);

        logOut<<$$->getRule()<<endl;
    }
    ;

expression_statement : SEMICOLON
    {
        Node* semicolonNode = new Node(@1.first_line,@1.last_line, true);
        semicolonNode->setRule("SEMICOLON : ;");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("expression_statement : SEMICOLON");
        $$->addChildNode(semicolonNode);

        logOut<<$$->getRule()<<endl;
    }
    | expression SEMICOLON
    {
        Node* semicolonNode = new Node(@2.first_line,@2.last_line, true);
        semicolonNode->setRule("SEMICOLON : ;");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("expression_statement : expression SEMICOLON");
        $$->addChildNode($1);
        $$->addChildNode(semicolonNode);

        logOut<<$$->getRule()<<endl;
    }
    ;

variable : ID 
    {
        Node* idNode = new Node(@1.first_line,@1.last_line, true);
        idNode->setRule("ID : "+$1->getName());

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("variable : ID");
        $$->addChildNode(idNode);

        SymbolInfo* idSymbol = symbolTable->Lookup($1->getName());
        if(idSymbol == nullptr){
            errorOut<<"Line# "<<@1.first_line<<": Undeclared variable \'"<<$1->getName()<<"\'\n";
            errorCount++;
        }

        logOut<<$$->getRule()<<endl;
    }
    | ID LSQUARE expression RSQUARE 
    {
        Node* idNode = new Node(@1.first_line,@1.last_line, true);
        idNode->setRule("ID : "+$1->getName());
        Node* lthirdNode = new Node(@2.first_line,@2.last_line, true);
        lthirdNode->setRule("LSQUARE : [");
        Node* rthirdNode = new Node(@4.first_line,@4.last_line, true);
        rthirdNode->setRule("RSQUARE : ]");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("variable : ID LSQUARE expression RSQUARE");
        $$->addChildNode(idNode);
        $$->addChildNode(lthirdNode);
        $$->addChildNode($3);
        $$->addChildNode(rthirdNode);

        SymbolInfo* idSymbol = symbolTable->Lookup($1->getName());
        if(idSymbol == nullptr){
            errorOut<<"Line# "<<@1.first_line<<": Undeclared variable \'"<<$1->getName()<<"\'\n";
            errorCount++;
        }
        else if(!idSymbol->getArrayStatus()){
            errorOut<<"Line# "<<@1.first_line<<": \'"<<$1->getName()<<"\' is not an array\n";
            errorCount++;
        }

        logOut<<$$->getRule()<<endl;
    }
    ;

expression : logic_expression
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("expression : logic_expression");
        $$->addChildNode($1);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    | variable ASSIGNOP logic_expression
    {
        Node* assignopNode = new Node(@2.first_line,@2.last_line, true);
        assignopNode->setRule("ASSIGNOP : =");

        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("expression : variable ASSIGNOP logic_expression");
        $$->addChildNode($1);
        $$->addChildNode(assignopNode);
        $$->addChildNode($3);

        if($1->getType() == "FLOAT" && $3->getType() == "INT"){
            // no problem
            $$->setType("FLOAT");
        }
        else if($1->getType() == "INT" && $3->getType() == "FLOAT"){
            errorOut<<"Line# "<<@1.first_line<<": Warning: possible loss of data in assignment of FLOAT to INT\n";
            errorCount++;
        }
        else if($1->getType() == $3->getType()){
            $$->setType($1->getType());
        }

        logOut<<$$->getRule()<<endl;
    }
    ;

logic_expression : rel_expression
    {
        $$ = new Node(@$.first_line, @$.last_line,false);
        $$->setRule("logic_expression : rel_expression");
        $$->addChildNode($1);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    | rel_expression LOGICOP rel_expression 
    {
        Node* logicopNode = new Node(@2.first_line,@2.last_line, true);
        logicopNode->setRule("LOGICOP : "+$2->getName());
        // aro kaj kora lagbe
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("logic_expression : rel_expression LOGICOP rel_expression");
        $$->addChildNode($1);
        $$->addChildNode(logicopNode);
        $$->addChildNode($3);

        $$->setType($1->getType());    

        logOut<<$$->getRule()<<endl;
    }
    ;

rel_expression : simple_expression
    {
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("rel_expression : simple_expression");
        $$->addChildNode($1);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    | simple_expression RELOP simple_expression
    {
        Node* relopNode = new Node(@2.first_line,@2.last_line, true);
        relopNode->setRule("RELOP : "+$2->getName());
        // aro kaj ase
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("rel_expression : simple_expression RELOP simple_expression");
        $$->addChildNode($1);
        $$->addChildNode(relopNode);
        $$->addChildNode($3);

        $$->setType($1->getType()); 

        logOut<<$$->getRule()<<endl;
    }
    ;

simple_expression : term 
    {
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("simple_expression : term");
        $$->addChildNode($1);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    | simple_expression ADDOP term 
    {
        Node* addopNode = new Node(@2.first_line,@$.last_line,true);
        addopNode->setRule("ADDOP : "+$2->getName());
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("simple_expression : simple_expression ADDOP term");
        $$->addChildNode($1);
        $$->addChildNode(addopNode);
        $$->addChildNode($3);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    ;

term : unary_expression
    {
        $$ = new Node(@1.first_line,@1.last_line,false);
        $$->setRule("term : unary_expression");
        $$->addChildNode($1);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    | term MULOP unary_expression
    {
        Node* mulopNode = new Node(@2.first_line,@2.last_line,true);
        mulopNode->setRule("MULOP : "+$2->getName());
        $$ = new Node(@1.first_line,@1.last_line,false);
        $$->setRule("term : term MULOP unary_expression");
        $$->addChildNode($1);
        $$->addChildNode(mulopNode);
        $$->addChildNode($3);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    ;

unary_expression : ADDOP unary_expression
    {
        Node* addopNode = new Node(@$.first_line,@$.last_line,true);
        addopNode->setRule("ADDOP : "+$1->getName());
        $$ = new Node(@$.first_line, @$.last_line, false);
        $$->setRule("unary_expression : ADDOP unary_expression");
        $$->addChildNode(addopNode);
        $$->addChildNode($2);
        $$->setType($2->getType());

        logOut<<$$->getRule()<<endl;
    }
    | NOT unary_expression
    {
        Node* notopNode = new Node(@$.first_line,@$.last_line,true);
        notopNode->setRule("NOTOP : "+$1->getName());
        $$ = new Node(@$.first_line, @$.last_line, false);
        $$->setRule("unary_expression : NOT unary_expression");
        $$->addChildNode(notopNode);
        $$->addChildNode($2);
        $$->setType($2->getType());

        logOut<<$$->getRule()<<endl;
    }
    | factor
    {
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("unary_expression : factor");
        $$->addChildNode($1);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    ;

factor : variable
    {
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("factor : variable");
        $$->addChildNode($1);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    | ID LPAREN argument_list RPAREN 
    {
        Node* idNode = new Node(@1.first_line,@1.last_line,true);
        idNode->setRule("ID : "+$1->getName());
        Node* lparenNode = new Node(@2.first_line,@2.last_line,true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@4.first_line,@4.last_line,true);
        rparenNode->setRule("RPAREN : )");

        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("factor : ID LPAREN argument_list RPAREN");
        $$->addChildNode(idNode);
        $$->addChildNode(lparenNode);
        $$->addChildNode($3);
        $$->addChildNode(rparenNode);

        SymbolInfo* idSymbol = symbolTable->Lookup($1->getName());
        if(idSymbol == nullptr){
            errorOut<<"Line# "<<@1.first_line<<": Undeclared function \'"<<$1->getName()<<"\'\n";
            errorCount++;
        }
        else if(idSymbol->getType() != "FUNCTION"){
            errorOut<<"Line# "<<@1.first_line<<": \'"<<$1->getName()<<"\' is not a function\n";
            errorCount++;
        }
        else{
            // Checking the number of parameters
            if(idSymbol->getParamList().size() < $3->getParameterList().size()){
                errorOut<<"Line# "<<@1.first_line<<": Too many arguments to function \'"<<$1->getName()<<"\'\n";
                errorCount++;
            }
            else if(idSymbol->getParamList().size() > $3->getParameterList().size()){
                errorOut<<"Line# "<<@1.first_line<<": Too few arguments to function \'"<<$1->getName()<<"\'\n";
                errorCount++;
            }
            // Checking the types of the parameters
            int nParameters = (idSymbol->getParamList().size() < $3->getParameterList().size())? idSymbol->getParamList().size() : $3->getParameterList().size();

            for(int i=0;i<nParameters;i++){
                if(idSymbol->getParamList()[i]->getDataType() != $3->getParameterList()[i]->getDataType()){
                    errorOut<<"Line# "<<@1.first_line<<": Type mismatch for argument "<<(i+1)<< " of \'"<<$1->getName()<<"\'\n";
                    errorCount++;
                }
            }
        }

        logOut<<$$->getRule()<<endl;
    }
    | LPAREN expression RPAREN 
    {
        Node* lparenNode = new Node(@1.first_line,@1.last_line,true);
        lparenNode->setRule("LPAREN : (");
        Node* rparenNode = new Node(@3.first_line,@3.last_line,true);
        rparenNode->setRule("RPAREN : )");

        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("factor : LPAREN expression RPAREN");
        $$->addChildNode(lparenNode);
        $$->addChildNode($2);
        $$->addChildNode(rparenNode);
        $$->setType($2->getType());

        logOut<<$$->getRule()<<endl;
    }
    | CONST_INT 
    {
        Node* const_intNode = new Node(@1.first_line,@1.last_line,true);
        const_intNode->setRule("CONST_INT : "+$1->getName());

        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("factor : CONST_INT");
        $$->addChildNode(const_intNode);
        $$->setType("INT");

        logOut<<$$->getRule()<<endl;
    }
    | CONST_FLOAT
    {
        Node* const_floatNode = new Node(@1.first_line,@1.last_line,true);
        const_floatNode->setRule("CONST_FLOAT : "+$1->getName());

        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("factor : CONST_FLOAT");
        $$->addChildNode(const_floatNode);
        $$->setType("FLOAT");

        logOut<<$$->getRule()<<endl;
    }
    | variable INCOP
    {
        Node* incopNode = new Node(@2.first_line,@2.last_line,true);
        incopNode->setRule("INCOP : "+$2->getName());

        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("factor : variable INCOP");
        $$->addChildNode($1);
        $$->addChildNode(incopNode);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    | variable DECOP
    {
        Node* decopNode = new Node(@2.first_line,@2.last_line,true);
        decopNode->setRule("DECOP : "+$2->getName());

        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("factor : variable DECOP");
        $$->addChildNode($1);
        $$->addChildNode(decopNode);
        $$->setType($1->getType());

        logOut<<$$->getRule()<<endl;
    }
    ;

argument_list : arguments 
    {
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("argument_list : arguments");
        $$->addChildNode($1);
        for(SymbolInfo* arg : $1->getParameterList()){
            $$->addParameter(arg);
        }

        logOut<<$$->getRule()<<endl;
    }
    | 
    {
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("argument_list : ");

        logOut<<$$->getRule()<<endl;
    }
    ;

arguments : arguments COMMA logic_expression
    {
        Node* commaNode = new Node(@2.first_line,@2.last_line,true);
        commaNode->setRule("COMMA : ,");

        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("arguments : arguments COMMA logic_expression");
        $$->addChildNode($1);
        $$->addChildNode(commaNode);
        $$->addChildNode($3);

        for(SymbolInfo* arg : $1->getParameterList()){
            $$->addParameter(arg);
        }
        // Jhamela hoite pare
        $$->addParameter(new SymbolInfo("","ARGUMENT",$3->getType()));

        logOut<<$$->getRule()<<endl;
    }
    | logic_expression
    {
        $$ = new Node(@$.first_line,@$.last_line,false);
        $$->setRule("arguments : logic_expression");
        $$->addChildNode($1);
        // Jhamela hoite pare
        $$->addParameter(new SymbolInfo("","ARGUMENT",$1->getType()));

        logOut<<$$->getRule()<<endl;
    }
    ;
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

    /* cout<<"File khultesi\n"; */
    logOut.open("2005006_log.txt");
    /* cout<<"Log khulse!\n"; */
    errorOut.open("2005006_error.txt");
    /* cout<<"Error khulse!\n"; */
    parseTreeOut.open("2005006_parseTree.txt");
    /* cout<<"ParseTree khulse!\n"; */

	yyin= fin;
	yyparse();
    /* symbolTable->printAll(logOut); */

    logOut<<"Total lines: "<<lineCount<<endl;
    logOut<<"Total errors: "<<errorCount<<endl;

    fclose(yyin);
    logOut.close();
    errorOut.close();
    parseTreeOut.close();
	return 0;
}
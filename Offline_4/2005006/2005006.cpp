#include<iostream>
#include<fstream>
#include<sstream>
#include <vector>
#include<cctype>
// Header portion of the assembly code
#define asmHeader ".MODEL SMALL\n\
.STACK 1000H\n\
.DATA\n\
\tCR EQU 0DH\n\
\tLF EQU 0AH\n\
\tNUMBER DB \"00000$\"\n"

// asm code for printing
#define printLibrary "new_line proc\n\
    \tpush ax\n\
    \tpush dx\n\
    \tmov ah,2\n\
    \tmov dl,0Dh\n\
    \tint 21h\n\
    \tmov ah,2\n\
    \tmov dl,0Ah\n\
    \tint 21h\n\
    \tpop dx\n\
    \tpop ax\n\
    \tret\n\
    \tnew_line endp\n\
print_output proc  ;print what is in ax\n\
    \tpush ax\n\
    \tpush bx\n\
    \tpush cx\n\
    \tpush dx\n\
    \tpush si\n\
    \tlea si,number\n\
    \tmov bx,10\n\
    \tadd si,4\n\
    \tcmp ax,0\n\
    \tjnge negate\n\
    \tprint:\n\
    \txor dx,dx\n\
    \tdiv bx\n\
    \tmov [si],dl\n\
    \tadd [si],'0'\n\
    \tdec si\n\
    \tcmp ax,0\n\
    \tjne print\n\
    \tinc si\n\
    \tlea dx,si\n\
    \tmov ah,9\n\
    \tint 21h\n\
    \tpop si\n\
    \tpop dx\n\
    \tpop cx\n\
    \tpop bx\n\
    \tpop ax\n\
    \tret\n\
    \tnegate:\n\
    \tpush ax\n\
    \tmov ah,2\n\
    \tmov dl,'-'\n\
    \tint 21h\n\
    \tpop ax\n\
    \tneg ax\n\
    \tjmp print\n\
    \tprint_output endp\n"
using namespace std;

class GlobalParameters
{
    int label_count = 0;
    string currentFunc;
    public:
        static GlobalParameters& getInstance()
        {
            static GlobalParameters instance;
            return instance;
        }
        string getCurrentFunc() {
            return currentFunc;
        }
        void setCurrentFunc(string func) {
            currentFunc = func;
        }
        int getLabelCount() {
            return ++label_count;
        }
        int showLabel() {
            return label_count;
        }  
};

class SymbolInfo{
    string Name; 
    string Type; 
    string dataType; 
    int bucketIndex;
    int blockIndex;
    int scopeTableID;
    SymbolInfo* next;
    bool isFuncDefn = false;
    bool isFuncDecl = false;
    bool isArray = false;
    bool isGlobal = false; // For global variables
    int arraySize = 0;
    int stackOffset = 0;
    vector<SymbolInfo*> paramList; 

    public:
        SymbolInfo(string name, string type){
            Name = name;
            Type = type;
            dataType = "";
            next = nullptr;
        }
        SymbolInfo(string name, string type, string datatype){
            Name = name;
            Type = type;
            dataType = datatype;
            next = nullptr;
        }
        SymbolInfo(SymbolInfo* SymbolInfo){
            Name = SymbolInfo->getName();
            Type = SymbolInfo->getType();
            dataType = SymbolInfo->getDataType();
            bucketIndex = SymbolInfo->getBucketIndex();
            blockIndex = SymbolInfo->getBlockIndex();
            scopeTableID = SymbolInfo->getScopeTableID();
            next = SymbolInfo->getNext();
            isFuncDefn = SymbolInfo->getFuncDefnStatus();
            isFuncDecl = SymbolInfo->getFuncDeclStatus();
            isArray = SymbolInfo->getArrayStatus();
            arraySize = SymbolInfo->getArraySize();
            paramList = SymbolInfo->getParamList();
            stackOffset = SymbolInfo->getStackOffset();
            isGlobal = SymbolInfo->getGlobalStatus();
        }
        string getName(){
            return Name;
        }
        void setName(string name){
            Name = name;
        }
        string getType(){
            return Type;
        }
        void setType(string type){
            Type = type;
        }
        string getDataType(){
            return dataType;
        }
        void setDataType(string datatype){
            dataType = datatype;
        }
        
        int getBucketIndex(){
            return bucketIndex;
        }
        void setBucketIndex(int index){
            bucketIndex = index;
        }
        int getBlockIndex(){
            return blockIndex;
        }
        void setBlockIndex(int index){
            blockIndex = index;
        }
        void decrementBlockIndex(){
            blockIndex--;
        }
        int getScopeTableID(){
            return scopeTableID;
        }
        void setScopeTableID(int ID){
            scopeTableID = ID;
        }
        SymbolInfo* getNext(){
            return next;
        }   
        void setNext(SymbolInfo* symbol){
            next = symbol;
        }
        bool getFuncDefnStatus(){
            return isFuncDefn;
        }
        void setFuncDefnStatus(bool status){
            isFuncDefn = status;
        }
        bool getFuncDeclStatus(){
            return isFuncDecl;
        }
        void setFuncDeclStatus(bool status){
            isFuncDecl = status;
        }
        bool getArrayStatus(){
            return isArray;
        }
        void setArrayStatus(bool status){
            isArray = status;
        }
        int getArraySize(){
            return arraySize;
        }
        void setArraySize(int size){
            arraySize = size;
        }
        int getStackOffset(){
            return stackOffset;
        }
        void setStackOffset(int offset){
            stackOffset = offset;
        }   

        bool getGlobalStatus(){
            return isGlobal;
        }

        void setGlobalStatus(bool status){
            isGlobal = status;
        }

        vector<SymbolInfo*> getParamList(){
            return paramList;
        }
        void setParamList(vector<SymbolInfo*> list){
            paramList = list;
        }
        void addParameter(SymbolInfo* symbol){
            paramList.push_back(symbol);
        }
};

class ScopeTable{
    SymbolInfo** buckets;
    int nBuckets;
    int ID;
    int count; // number of subordinate classes
    int stackOffset = 0;

    unsigned long long sdbm(string str)
    {
        unsigned long long hash = 0;

        for (char c : str){
            hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash%nBuckets;
    }

    void decrementBlockIndicesOfNextSymbols(SymbolInfo* symbol){
        if(symbol == nullptr){
            return;
        }
        while(symbol != nullptr){
            symbol->decrementBlockIndex();
            symbol = symbol->getNext();
        }
    }

    public:
        ScopeTable* parentScope;
        ScopeTable(int n){
            nBuckets = n;
            parentScope = nullptr;
            buckets = new SymbolInfo*[nBuckets];
            for(int i=0;i<nBuckets;i++){
                buckets[i] = nullptr;
            }
            count = 0;
        }
        ~ScopeTable(){
            for(int i=0;i<nBuckets;i++){
                SymbolInfo* current = buckets[i];
                SymbolInfo* temp;
                while(current != nullptr){
                    temp = current;
                    current = current->getNext();
                    delete temp;
                }
            }
            delete[] buckets;
        }
        
        bool Insert(string name, string type, string datatype){
            if(LookUp(name) == nullptr){
                int i = (int)sdbm(name);
                int j = 1;
                SymbolInfo* current = buckets[i];
                if(current == nullptr){
                    current = new SymbolInfo(name,type,datatype);
                    buckets[i] = current;
                }
                else{
                    SymbolInfo* parent = current;
                    while(current != nullptr){
                        parent = current;
                        current = current->getNext();
                        j++;
                    }
                    current = new SymbolInfo(name,type,datatype);
                    parent->setNext(current);
                }
                // cout<<"\tInserted  at position <"<<i+1<<", "<<j<<"> of ScopeTable# "<<ID<<endl; // 1 based indexing
                current->setBucketIndex(i+1);
                current->setBlockIndex(j);
                current->setScopeTableID(ID);
                
                return true;
            }
            // cout<<"\t'"<<name<<"' already exists in the current ScopeTable# "<<ID<<endl;
            return false;
        }

        bool Insert(SymbolInfo* symbol){
            if(LookUp(symbol->getName()) == nullptr){
                int i = (int)sdbm(symbol->getName());
                int j = 1;
                SymbolInfo* current = buckets[i];
                if(current == nullptr){
                    current = symbol;
                    buckets[i] = current;
                }
                else{
                    SymbolInfo* parent = current;
                    while(current != nullptr){
                        parent = current;
                        current = current->getNext();
                        j++;
                    }
                    current = symbol;
                    parent->setNext(current);
                }
                // cout<<"\tInserted  at position <"<<i+1<<", "<<j<<"> of ScopeTable# "<<ID<<endl; // 1 based indexing
                current->setBucketIndex(i+1);
                current->setBlockIndex(j);
                current->setScopeTableID(ID);
                
                return true;
            }
            // cout<<"\t'"<<name<<"' already exists in the current ScopeTable# "<<ID<<endl;
            return false;
        }
        
        SymbolInfo* LookUp(string name){
            SymbolInfo* current = nullptr;
            for(int i=0;i<nBuckets;i++){
                current = buckets[i];
                while(current != nullptr){
                    if(name == current->getName()){
                        return current;
                    }
                    current = current->getNext();
                }
            }
            return current;
        }

        bool Delete(string name){
            SymbolInfo* current = LookUp(name);
            if(current == nullptr){
                // cout<<"\tNot found in the current ScopeTable# "<<ID<<endl;
                return false;
            }
            
            int i = current->getBucketIndex() - 1; //1 based to 0 based index;
            int j = current->getBlockIndex() - 1; //1 based to 0 based index;
            if(j==0){
                buckets[i] = current->getNext();
            }
            else{
                SymbolInfo* parent = buckets[i];
                for(int count=0;count<j-1;count++){
                    parent = parent->getNext();
                }
                parent->setNext(current->getNext());
            }
            // cout<<"\tDeleted '"<<name<<"' from position <"<<i+1
            // <<", "<<j+1<<"> of ScopeTable# "<<ID<<endl;
            decrementBlockIndicesOfNextSymbols(current->getNext());
            delete current;
            return true;
        }

        void Print(ofstream& out){
            out<<"\tScopeTable# "<<ID<<endl;
            for(int i=0;i<nBuckets;i++){
                SymbolInfo* current = buckets[i];
                if(current == nullptr){
                    cout<<endl;
                }
                else{
                    out<<"\t"<<i+1<<"-->";
                    while(current != nullptr){
                        if(current->getType() == "FUNCTION"){
                            out<<" <"<<current->getName()<<","<<current->getType()<<","<<current->getDataType()<<">";
                        }
                        else if(current->getType() == "ARRAY"){
                            out<<" <"<<current->getName()<<","<<current->getType()<<">";
                        }
                        else{
                            out<<" <"<<current->getName()<<","<<current->getDataType()<<">";
                        } 
                        current = current->getNext();
                    }
                    out<<endl;
                }
            }
        }
        int getID(){
            return ID;
        }
        void setID(int id){
            ID = id;
        }
        int getCount(){
            return count;
        }
        int incrementCount(){
            return ++count;
        }
        int getStackOffset(){
            return stackOffset;
        }
        void setStackOffset(int offset){
            stackOffset = offset;
        }
};

class SymbolTable{
    int nBuckets;
    int scopeTableID;
    public:
        ScopeTable* currentScope;
        SymbolTable(int n){
            nBuckets = n;
            currentScope = new ScopeTable(nBuckets);
            currentScope->setID(1);
            scopeTableID = 1;
            currentScope->parentScope = nullptr;
            // out<<"\tScopeTable# "<<currentScope->getID()<<" created\n";
        }
        ~SymbolTable(){
            while(currentScope != nullptr){
                ScopeTable* temp = currentScope->parentScope;
                int ID = currentScope->getID();
                delete currentScope;
                currentScope = temp;
                // out<<"\tScopeTable# "<<ID<<" deleted\n";
            }
        }
        void enterScope(){
            scopeTableID++;
            ScopeTable* newScope = new ScopeTable(nBuckets);
            newScope->setID(scopeTableID);
            newScope->parentScope = currentScope;
            currentScope = newScope;
            
            // cout<<"\tScopeTable# "<<id<<" created\n";
        }
        void exitScope(){
            // string id = currentScope->getID();
            if(currentScope->parentScope == nullptr){
                // cout<<"\tScopeTable# "<< id <<" cannot be deleted\n";
            }
            else{
                ScopeTable* temp = currentScope->parentScope;
                delete currentScope;
                currentScope = temp;
                // cout<<"\tScopeTable# "<< id <<" deleted\n";
            }
        }

        int getCurrentScopeID(){
            return currentScope->getID();
        }

        bool Insert(string name,string type,string datatype){
            return currentScope->Insert(name,type,datatype);
        }
        bool Insert(SymbolInfo* symbol){
            return currentScope->Insert(symbol);
        }
        bool Remove(string name){
            return currentScope->Delete(name);
        }
        SymbolInfo* Lookup(string name){
            ScopeTable* current = currentScope;
            
            SymbolInfo* symbol = nullptr;
            while(current != nullptr){
                symbol = current->LookUp(name);
                if(symbol == nullptr){
                    if(current->parentScope == nullptr){
                        break;
                    }
                    current = current->parentScope;
                }
                else{
                    // cout<<"\t'"<<symbol->getName()<<"' found at position <"<<symbol->getBucketIndex()<<", "<<symbol->getBlockIndex()<<"> of ScopeTable# "<<current->getID()<<endl;
                    return symbol;
                }
            }
            // out<<"\t'"<<name<<"' not found in any of the ScopeTables\n";
            return nullptr;
        }
        SymbolInfo* LookupGlobalScope(string name){
            ScopeTable* current = currentScope;
            if(current->parentScope != nullptr){
                current = current->parentScope;
            }
            return current->LookUp(name);
        }
        void printCurrent(ofstream& out){
            currentScope->Print(out);
        }
        void printAll(ofstream& out){
            ScopeTable* current = currentScope;
            while(current != nullptr){
                current->Print(out);
                current = current->parentScope;
            }
        }

        int getStackOffset(){
            int offset = 0;
            ScopeTable* current = currentScope;
            while(current != nullptr){
                offset += current->getStackOffset();
                current = current->parentScope;
            }
            return offset;
        }

        int getCurrentScopeStackOffset(){
            return currentScope->getStackOffset();
        }

        void setCurrentScopeStackOffset(int offset){
            currentScope->setStackOffset(offset);
        }

};


// string currentFunctionName = "";

class Node{
    int startLine, endLine;
    string type;
    string rule;
    vector<Node*> childrenNodes;
    vector<SymbolInfo*> parameterList; // For functions
    vector<SymbolInfo*> globalVariables;
    int stackOffset = 0;
    bool isGlobal = false;

    // string getNameFromRule(string rule){
    //     //Rule for ID----> ID : Name
    //     int colonPos = rule.find(":");
    //     string name = rule.substr(colonPos+2);
    //     return name;
    // }

    // string getNameFromRule2(string rule){
    //     //Rule for ID----> ID : Name
    //     int colonPos = rule.find(":");
    //     string name = rule.substr(colonPos+1);
    //     return name;
    // }

    string getNewLabel(){
        // 1st label would be L1 and so on
        int labelCount = GlobalParameters::getInstance().getLabelCount();
        return "L"+to_string(labelCount);
    }

    string getASMForRelop(string relop){
        if(relop == "<"){
            return "\tJL";
        }
        else if(relop == "<="){
            return "\tJLE";
        }
        else if(relop == ">"){
            return "\tJG";
        }
        else if(relop == ">="){
            return "\tJGE";
        }
        else if(relop == "=="){
            return "\tJE";
        }
        else if(relop == "!="){
            return "\tJNE";
        }
        return "";
    }

    public:
        string name = "";
        // static int labelCount;
        string trueLabel = "";
        string falseLabel = "";
        string endLabel = "";
        bool hasPrintStatement = false;
        bool isToken = false; // For checking if the node is a token eg. ID, COMMA, SEMICOLON etc
        bool isCondition = false; 
        bool isArray = false;
        Node(int startLine, int endLine, bool isToken){
            this->startLine = startLine;
            this->endLine = endLine;
            this->isToken = isToken; 
            // cout<<startLine<<" "<<endLine<<endl;
        }
        ~Node(){
            
        }

        int getStartLine(){
            return startLine;
        }
        void setStartLine(int startLine){
            this->startLine = startLine;
        }

        int getEndLine(){
            return endLine;
        }
        void setEndLine(int endLine){
            this->endLine = endLine;
        }

        string getType(){
            return type;
        }
        void setType(string type){
            this->type = type;
        }

        string getRule(){
            return rule;
        }
        void setRule(string rule){
            this->rule = rule;
        }

        int getStackOffset(){
            return stackOffset;
        }

        void setStackOffset(int offset){
            stackOffset = offset;
        }

        bool getGlobalStatus(){
            return isGlobal;
        }

        void setGlobalStatus(bool status){
            isGlobal = status;
        }

        vector<Node*> getChildrenNodes(){
            return childrenNodes;
        }
        void setChildrenNodes(vector<Node*> childrenNodes){
            this->childrenNodes = childrenNodes;
        }

        void addChildNode(Node* childNode){
            childrenNodes.push_back(childNode);
        }

        vector<SymbolInfo*> getParameterList(){
            return parameterList;
        }
        void setParameterList(vector<SymbolInfo*> parameterList){
            this->parameterList = parameterList;
        }
        void addParameter(SymbolInfo* type){
            parameterList.push_back(type);
        }

        vector<SymbolInfo*> getGlobalVariables(){
            return globalVariables;
        }

        void setGlobalVariables(vector<SymbolInfo*> globalVariables){
            this->globalVariables = globalVariables;
        }

        void printParseTree(ofstream& out, Node* root, int level){
            if(root == nullptr){
                return;
            }
            // Space for indentation
            for(int i=1; i< level ; i++){
                out<<" ";
            }

            // \t diye nao milte pare!!
            if(root->isToken){
                // Endline is not printed for tokens
                out<<root->getRule()<<"\t <Line: "<<root->getStartLine()<<">\n";
            }
            else{
                out<<root->getRule()<<"\t <Line: "<<root->getStartLine()<<"-"<<root->getEndLine()<<">\n";
            }
            
            for(Node* childNode : root->getChildrenNodes()){
                printParseTree(out, childNode, level+1);
            }

        }

        void generateCode(ofstream& out){
            if(rule == "start : program"){
                // adding the header portion of the assembly code
                out<<asmHeader;
                // adding the global variables in the data section
                for(SymbolInfo* var : globalVariables){
                    if(var->getArrayStatus()){
                        out<<"\t"<<var->getName()<<" DW "<<var->getArraySize()<<" DUP (0000H)\n";
                    }
                    else{
                        out<<"\t"<<var->getName()<<" DW 1 DUP (0000H)\n";
                    }
                }

                out<<".CODE\n";
                childrenNodes[0]->generateCode(out);

                if(hasPrintStatement){
                    out<<printLibrary;
                }

                out<<"END main\n";
            }

            else if(rule == "program : program unit"){
                childrenNodes[0]->generateCode(out);
                childrenNodes[1]->generateCode(out);
            }

            else if(rule == "program : unit"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "unit : var_declaration"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "unit : func_declaration"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "unit : func_definition"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON"){
                childrenNodes[0]->generateCode(out);
                childrenNodes[3]->generateCode(out);
            }

            else if(rule == "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement"){
                childrenNodes[0]->generateCode(out);
                childrenNodes[3]->generateCode(out);

                //Adding name field would require changing a lot of code, hence this :")
                string idName = childrenNodes[1]->name;    
                GlobalParameters::getInstance().setCurrentFunc(idName);
                cout<<idName<<endl;

                out<<idName<<" PROC\n";
                if(idName == "main"){
                    // Loading the data segment
                    out<<"\tMOV AX, @DATA\n";
                    out<<"\tMOV DS, AX\n";
                }

                // Preparing to use the stack
                out<<"\tPUSH BP\n";
                out<<"\tMOV BP, SP\n";

                // Function body
                childrenNodes[5]->generateCode(out);

                out<<idName<<"_EXIT:\n";
                // Stack offset er kaj baki
                out<<"\tADD SP, "<<childrenNodes[5]->getStackOffset()<<"\n";
                out<<"\tPOP BP\n";

                if(idName == "main"){
                    out<<"\tMOV AH, 4CH\n";
                    out<<"\tINT 21H\n";
                }
                else{
                    if(childrenNodes[0]->getType()=="VOID"){
                        out<<"\tRET\n";
                    }
                    else{
                        out<<"\tRET "<<childrenNodes[3]->getParameterList().size()*2<<"\n"; 
                    }
                }
                out<<idName<<" ENDP\n";
            }

            else if(rule == "func_definition : type_specifier ID LPAREN RPAREN compound_statement"){
                childrenNodes[0]->generateCode(out);
                //Adding name field would require changing a lot of code, hence this :")
                string idName = childrenNodes[1]->name;
                GlobalParameters::getInstance().setCurrentFunc(idName);
                out<<idName<<" PROC\n";
                if(idName == "main"){
                    // Loading the data segment
                    out<<"\tMOV AX, @DATA\n";
                    out<<"\tMOV DS, AX\n";
                }

                // Preparing to use the stack
                out<<"\tPUSH BP\n";
                out<<"\tMOV BP, SP\n";

                // Function body
                childrenNodes[4]->generateCode(out);

                out<<idName<<"_EXIT:\n";
                // Stack offset er kaj baki
                out<<"\tADD SP, "<<childrenNodes[4]->getStackOffset()<<"\n";
                out<<"\tPOP BP\n";

                if(idName == "main"){
                    out<<"\tMOV AH, 4CH\n";
                    out<<"\tINT 21H\n";
                }
                else{
                    out<<"\tRET\n";
                }
                out<<idName<<" ENDP\n";
            }
            else if(rule == "parameter_list : parameter_list COMMA type_specifier ID"){
                childrenNodes[0]->generateCode(out);
                childrenNodes[2]->generateCode(out);
            }

            else if(rule == "parameter_list : parameter_list COMMA type_specifier"){
                childrenNodes[0]->generateCode(out);
                childrenNodes[2]->generateCode(out);
            }

            else if(rule == "parameter_list : type_specifier ID"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "parameter_list : type_specifier"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "compound_statement : LCURL statements RCURL"){
                if(endLabel == ""){
                    endLabel = getNewLabel();
                }
                childrenNodes[1]->endLabel = endLabel;
                childrenNodes[1]->generateCode(out);
            }

            else if(rule == "compound_statement : LCURL RCURL"){
                if(endLabel == ""){
                    endLabel = getNewLabel();
                }
                // There is no code to generate
            }

            else if(rule == "var_declaration : type_specifier declaration_list SEMICOLON"){
                childrenNodes[0]->generateCode(out);
                childrenNodes[1]->generateCode(out);

                for(SymbolInfo* var : childrenNodes[1]->getParameterList()){
                    //Global variables are declared earlier in the Data segment
                    if(!var->getGlobalStatus()){
                        if(var->getArrayStatus()){
                            out<<"\tSUB SP, "<<to_string(var->getArraySize()*2)<<endl;
                        }
                        else{
                            out<<"\tSUB SP, 2\n";
                        }
                    }
                }
            }
            
            else if(rule == "statements : statement"){
                if(endLabel == "") {
                    endLabel = getNewLabel();
                }
                childrenNodes[0]->endLabel = endLabel;
                childrenNodes[0]->generateCode(out);
                out<<endLabel<<":\n";
            }

            else if(rule == "statements : statements statement"){
                if(endLabel == "") {
                    endLabel = getNewLabel();
                }
                childrenNodes[0]->endLabel = getNewLabel();
                childrenNodes[1]->endLabel = endLabel;
                
                childrenNodes[0]->generateCode(out);
                childrenNodes[1]->generateCode(out);

                out<<endLabel<<":\n";
            }

            else if(rule == "statement : var_declaration"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "statement : expression_statement"){
               childrenNodes[0]->generateCode(out); 
            }

            else if(rule == "statement : compound_statement"){
                childrenNodes[0]->endLabel = getNewLabel();
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement"){
                childrenNodes[2]->generateCode(out);
                string loopLabel = getNewLabel();
                out<<loopLabel<<":\n";
                // 2nd expression_statement denotes the condition in a for loop
                childrenNodes[3]->isCondition = true;
                childrenNodes[3]->trueLabel = getNewLabel();
                // .......
                if(endLabel == "") {
                    endLabel = getNewLabel();
                }
                childrenNodes[3]->falseLabel = endLabel;
                // generate code for the condition
                childrenNodes[3]->generateCode(out); 
                out<<childrenNodes[3]->trueLabel<<":\n";
                childrenNodes[6]->endLabel = endLabel;
                //generate code for the function body    
                childrenNodes[6]->generateCode(out); 
                // generate code for the loop iterator change code
                childrenNodes[4]->generateCode(out);
                // Go to the condition checking
                out<<"\tJMP "<<loopLabel<<endl;
                // Pore change hoite pare
                // out<<childrenNodes[3]->falseLabel<<endl;
            }

            else if(rule == "statement : IF LPAREN expression RPAREN statement"){
                childrenNodes[2]->isCondition = true;
                childrenNodes[2]->trueLabel = getNewLabel();
                if(endLabel == "") {
                    endLabel = getNewLabel();
                }
                childrenNodes[2]->falseLabel = endLabel;
                childrenNodes[2]->generateCode(out);
                out<<childrenNodes[2]->trueLabel<<":\n";

                childrenNodes[4]->endLabel = endLabel;
                childrenNodes[4]->generateCode(out);
            }

            else if(rule == "statement : IF LPAREN expression RPAREN statement ELSE statement"){
                childrenNodes[2]->isCondition = true;
                childrenNodes[2]->trueLabel = getNewLabel();
                childrenNodes[2]->falseLabel = getNewLabel();

                childrenNodes[2]->generateCode(out);

                if(endLabel == "") {
                    endLabel = getNewLabel();
                }

                out<<childrenNodes[2]->trueLabel<<":\n";
                childrenNodes[4]->endLabel = endLabel;
                childrenNodes[4]->generateCode(out);
                out<<"\tJMP "<<childrenNodes[4]->endLabel<<endl;

                out<<childrenNodes[2]->falseLabel<<":\n";
                childrenNodes[6]->endLabel = endLabel;
                childrenNodes[6]->generateCode(out);
            }

            else if(rule == "statement : WHILE LPAREN expression RPAREN statement"){
                string loopLabel = getNewLabel();
                childrenNodes[2]->isCondition = true;
                childrenNodes[2]->trueLabel = getNewLabel();
                if(endLabel == "") {
                    endLabel = getNewLabel();
                }
                childrenNodes[2]->falseLabel = endLabel;

                out<<loopLabel<<":\n";
                childrenNodes[2]->generateCode(out);

                out<<childrenNodes[2]->trueLabel<<":\n";
                childrenNodes[4]->endLabel = endLabel; 
                childrenNodes[4]->generateCode(out);
                out<<"\tJMP "<<loopLabel<<endl;
            }

            else if(rule == "statement : PRINTLN LPAREN ID RPAREN SEMICOLON"){
                if(isGlobal){
                    out<<"\tMOV AX, "<<childrenNodes[2]->name<<endl;
                    out<<"\tCALL print_output\n";
                    out<<"\tCALL new_line\n";
                }
                else{
                    //Here the stack offset of the statement is set to be the stack offset of the ID
                    out<<"\tMOV AX, [BP-"<<to_string(stackOffset)<<"]\n";
                    out<<"\tCALL print_output\n";
                    out<<"\tCALL new_line\n";
                    //add push pop BP if code misbehaves
                }
            }

            else if(rule == "statement : RETURN expression SEMICOLON"){
                childrenNodes[1]->generateCode(out);
                string currFunc = GlobalParameters::getInstance().getCurrentFunc();
                out<<"\tJMP "<<currFunc<<"_EXIT\n";
            }

            else if(rule == "expression_statement : expression SEMICOLON"){
                childrenNodes[0]->isCondition = isCondition;
                childrenNodes[0]->trueLabel = trueLabel;
                childrenNodes[0]->falseLabel = falseLabel;
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "variable : ID"){
                if(!isGlobal){
                    // Storing value of BP, so that it can be retrived later
                    out<<"\tPUSH BP\n";
                    out<<"\tMOV BX, "<<to_string(stackOffset)<<endl;
                    out<<"\tSUB BP, BX\n";
                    // Now BP points to the address of the variable
                }
            }

            else if(rule == "variable : ID LSQUARE expression RSQUARE"){
                childrenNodes[2]->generateCode(out);
                if(isGlobal){
                    out<<"\tLEA SI, "<<childrenNodes[0]->name<<endl;
                    // index is stored in CX, adding the double of it as datatype is int = 2 bytes
                    out<<"\tADD SI, CX\n";
                    out<<"\tADD SI, CX\n";
                    out<<"\tPUSH BP\n";
                    out<<"\tMOV BP, SI\n";
                }
                else{
                    out<< "\tPUSH BP\n";
                    out<< "\tMOV BX, CX\n";
                    out<< "\tADD BX, BX\n";
                    out<< "\tSUB BX, "<< to_string(stackOffset)<< "\n";
                    out<< "\tADD BP, BX\n";
                }
            }

            else if(rule == "expression : logic_expression"){
                childrenNodes[0]->isCondition = isCondition;   
                childrenNodes[0]->trueLabel = trueLabel;
                childrenNodes[0]->falseLabel = falseLabel;
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "expression : variable ASSIGNOP logic_expression"){
                childrenNodes[2]->generateCode(out);
                childrenNodes[0]->isCondition  = false;
                childrenNodes[0]->isCondition = false;
                if(childrenNodes[0]->getGlobalStatus() && !childrenNodes[0]->isArray){
                    childrenNodes[0]->generateCode(out);
                    out<<"\tMOV "<<childrenNodes[0]->name<<", CX\n";
                }
                else{
                    // might need optimization
                    out<<"\tPUSH CX\n";
                    childrenNodes[0]->generateCode(out);

                    out<<"\tPOP AX\n";
                    out<<"\tPOP CX\n";
                    out<<"\tMOV [BP], CX\n";
                    out<<"\tMOV BP, AX\n";
                }
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    out<<"\tJMP "<<trueLabel<<endl;
                }
            }

            else if(rule == "logic_expression : rel_expression"){
                childrenNodes[0]->isCondition = isCondition;
                if(trueLabel == ""){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""){
                    falseLabel = getNewLabel();
                }
                childrenNodes[0]->trueLabel = trueLabel;
                childrenNodes[0]->falseLabel = falseLabel;
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "logic_expression : rel_expression LOGICOP rel_expression"){
                childrenNodes[0]->isCondition = isCondition;
                childrenNodes[2]->isCondition = isCondition;

                if(trueLabel == ""){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""){
                    falseLabel = getNewLabel();
                }

                if(childrenNodes[1]->name == "||"){
                    childrenNodes[0]->trueLabel = trueLabel;
                    // When 1st condition is false
                    childrenNodes[0]->falseLabel = getNewLabel();
                    childrenNodes[2]->trueLabel = trueLabel;
                    childrenNodes[2]->falseLabel = falseLabel;
                }
                else{
                    // and operation
                    // When 1st condition is true
                    childrenNodes[0]->trueLabel = getNewLabel(); 
                    childrenNodes[0]->falseLabel = falseLabel;
                    childrenNodes[2]->trueLabel = trueLabel;
                    childrenNodes[2]->falseLabel = falseLabel;
                }
                childrenNodes[0]->generateCode(out);

                if(isCondition){
                    if(childrenNodes[1]->name == "||"){
                        //2nd condition is checked only if 1st is false
                        out<<childrenNodes[0]->falseLabel<<":\n";
                    }
                    else{
                        //2nd condition is checked only if 1st is true
                        out<<childrenNodes[0]->trueLabel<<":\n";
                    }
                }
                else out<<"\tPUSH CX\n";
                childrenNodes[2]->generateCode(out);
                if(!isCondition){
                    out<<"\tPOP AX\n";
                    if(childrenNodes[1]->name == "||"){
                        // OR operation
                        string firstFalse = getNewLabel();
                        string firstTrue = getNewLabel();
                        string secondFalse = getNewLabel();
                        string final = getNewLabel();

                        out<<"\tCMP AX, 0\n"; //Previous CX(1st rel_exp) is in AX
                        out<<"\tJE "<<firstFalse<<endl;
                        out<<"\tJMP "<<firstTrue<<endl;

                        out<<firstFalse<<":\n";
                        out<<"\tJCXZ "<<secondFalse<<endl; //If Current CX(2nd rel_exp)=0, final = 0

                        out<<firstTrue<<":\n";
                        out<<"\tMOV CX, 1\n"; //If 1st rel_exp is true, final = 1
                        out<<"\tJMP "<<final<<endl; 

                        out<<secondFalse<<":\n";
                        out<<"\tMOV CX, 0\n"; //If 2nd rel_exp is false, final = 0

                        out<<final<<":\n";
                    }
                    else{
                        //AND operation
                        string False = getNewLabel();
                        string firstTrue = getNewLabel();
                        string final = getNewLabel();

                        out<<"\tCMP AX, 0\n"; //Previous CX(1st rel_exp) is in AX
                        out<<"\tJE "<<False<<endl;
                        out<<"\tJMP "<<firstTrue<<endl;

                        out<<False<<":\n";
                        out<<"\tMOV CX, 0\n"; //If 1st rel_exp = 0 , final = 0
                        out<<"\tJMP "<<final<<endl;

                        out<<firstTrue<<":\n";
                        out<<"\tJCXZ "<<False<<endl; //If 2nd rel_exp = 0, final = 0
                        out<<"\tMOV CX, 1\n"; //If 1st & 2nd rel_exp = 1, final = 1

                        out<<final<<":\n";
                    }
                }
            }

            else if(rule == "rel_expression : simple_expression"){
                if(trueLabel == ""){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""){
                    falseLabel = getNewLabel();
                }
                childrenNodes[0]->isCondition = isCondition;
                childrenNodes[0]->trueLabel = trueLabel;
                childrenNodes[0]->falseLabel = falseLabel;
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "rel_expression : simple_expression RELOP simple_expression"){
                childrenNodes[0]->generateCode(out);
                out<<"\tPUSH CX\n";
                childrenNodes[2]->generateCode(out);
                string relop = childrenNodes[1]->name;
                string asmForRelop = getASMForRelop(relop);
                out<<"\tPOP AX\n";
                out<<"\tCMP AX, CX\n";
                if(trueLabel == "" || !isCondition){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""|| !isCondition){
                    falseLabel = getNewLabel();
                }
                out<<asmForRelop<<" "<<trueLabel<<endl;
                out<<"\tJMP "<<falseLabel<<endl;

                if(!isCondition){
                    out<<trueLabel<<":\n";
                    out<<"\tMOV CX, 1\n";
                    string exit = getNewLabel();
                    out<<"\tJMP "<<exit<<endl;
                    out<<falseLabel<<":\n";
                    out<<"\tMOV CX, 0\n";
                    out<<exit<<":\n";
                }
            }

            else if(rule == "simple_expression : term"){
                if(trueLabel == ""){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""){
                    falseLabel = getNewLabel();
                }
                childrenNodes[0]->isCondition = isCondition;
                childrenNodes[0]->trueLabel = trueLabel;
                childrenNodes[0]->falseLabel = falseLabel;
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "simple_expression : simple_expression ADDOP term"){
                childrenNodes[0]->generateCode(out);
                out<<"\tPUSH CX\n";
                childrenNodes[2]->generateCode(out);
                out<<"\tPOP AX\n";
                string addop = childrenNodes[1]->name;
                if(addop == "+"){
                    out<<"\tADD CX, AX\n";
                    //Ans is in CX
                }
                else{
                    out<<"\tSUB AX, CX\n";
                    out<<"\tMOV CX, AX\n";
                    //Ans is in CX
                }
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    // CX = 0 -> false, CX != 0 -> true
                    out<<"\tJCXZ "<<falseLabel<<endl;
                    out<<"\tJMP "<<trueLabel<<endl;
                }
            }

            else if(rule == "term : unary_expression"){
                if(trueLabel == ""){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""){
                    falseLabel = getNewLabel();
                }
                childrenNodes[0]->isCondition = isCondition;
                childrenNodes[0]->trueLabel = trueLabel;
                childrenNodes[0]->falseLabel = falseLabel;
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "term : term MULOP unary_expression"){
                childrenNodes[0]->generateCode(out);
                out<<"\tPUSH CX\n";
                childrenNodes[2]->generateCode(out);
                out<<"\tPOP AX\n";
                string mulop = childrenNodes[1]->name;
                if(mulop == "*"){
                    //signed multiplication
                    // out<<"\tCWD\n"; //sign extension
                    out<<"\tMUL CX\n";
                    out<<"\tMOV CX, AX\n"; //Ans moved to CX
                }
                else if(mulop == "/"){
                    //signed division
                    out<<"\tCWD\n"; //sign extension
                    out<<"\tIDIV CX\n";
                    out<<"\tMOV CX, AX\n"; //Ans moved to CX
                }
                else if(mulop == "%"){
                    //signed remainder
                    out<<"\tCWD\n"; //sign extension
                    out<<"\tIDIV CX\n";
                    out<<"\tMOV CX, DX\n"; //Ans moved to CX
                }
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    // CX = 0 -> false, CX != 0 -> true
                    out<<"\tJCXZ "<<falseLabel<<endl;
                    out<<"\tJMP "<<trueLabel<<endl;
                }
            }

            else if(rule == "unary_expression : ADDOP unary_expression"){
                if(trueLabel == ""){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""){
                    falseLabel = getNewLabel();
                }
                childrenNodes[1]->isCondition = isCondition;
                childrenNodes[1]->trueLabel = trueLabel;
                childrenNodes[1]->falseLabel = falseLabel;
                childrenNodes[1]->generateCode(out);

                if(childrenNodes[0]->name == "-"){
                    out<<"\tNEG CX\n";
                }
            }

            else if(rule == "unary_expression : NOT unary_expression"){
                if(trueLabel == ""){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""){
                    falseLabel = getNewLabel();
                }
                childrenNodes[1]->isCondition = isCondition;
                childrenNodes[1]->trueLabel = falseLabel;
                childrenNodes[1]->falseLabel = trueLabel;
                childrenNodes[1]->generateCode(out);

                if(!isCondition){
                    string falseButTrue = getNewLabel();
                    string exit = getNewLabel();

                    out<<"\tJCXZ "<<falseButTrue<<endl;
                    out<<"\tMOV CX, 0\n";
                    out<<"\tJMP "<<exit<<endl;
                    out<<falseButTrue<<":\n";
                    out<<"\tMOV CX, 1\n";
                    out<<exit<<":\n";
                }
            }

            else if(rule == "unary_expression : factor"){
                if(trueLabel == ""){
                    trueLabel = getNewLabel();
                }
                if(falseLabel == ""){
                    falseLabel = getNewLabel();
                }
                childrenNodes[0]->isCondition = isCondition;
                childrenNodes[0]->trueLabel = trueLabel;
                childrenNodes[0]->falseLabel = falseLabel;
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "factor : variable"){
                childrenNodes[0]->generateCode(out);
                if(childrenNodes[0]->getGlobalStatus() && !childrenNodes[0]->isArray){
                    out<<"\tMOV CX, "<<childrenNodes[0]->name<<endl;
                }
                else{
                    out<<"\tMOV CX, [BP]\n";
                    out<<"\tPOP BP\n";
                }
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    out<<"\tJCXZ "<<falseLabel<<":\n";
                    out<<"\tJMP "<<trueLabel<<":\n";
                }
            }

            else if(rule == "factor : ID LPAREN argument_list RPAREN"){
                childrenNodes[2]->generateCode(out);
                //Function call
                out<<"\tCALL "<<childrenNodes[0]->name<<endl;
                // out<<"\tADD SP, "<<childrenNodes[2]->getStackOffset()<<endl;
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    out<<"\tJCXZ "<<falseLabel<<":\n";
                    out<<"\tJMP "<<trueLabel<<":\n";
                }
            }

            else if(rule == "factor : LPAREN expression RPAREN"){
                childrenNodes[1]->generateCode(out);
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    out<<"\tJCXZ "<<falseLabel<<":\n";
                    out<<"\tJMP "<<trueLabel<<":\n";
                }
            }

            else if(rule == "factor : CONST_INT"){
                out<<"\tMOV CX, "<<childrenNodes[0]->name<<endl;
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    out<<"\tJCXZ "<<falseLabel<<endl;
                    out<<"\tJMP "<<trueLabel<<endl;
                }
            }

            else if(rule == "factor : variable INCOP"){
                childrenNodes[0]->generateCode(out);
                string varName = childrenNodes[0]->name;
                if(childrenNodes[0]->getGlobalStatus() && !childrenNodes[0]->isArray){
                    out<<"\tMOV CX "<<varName<<endl;
                }
                else{
                    out<<"\tMOV CX, [BP]\n";
                }
                out<<"\tMOV AX, CX\n"; // Storing CX's value for condition checking

                out<<"\tINC CX\n";
                // update for test case 6
                if(childrenNodes[0]->getGlobalStatus()&& !childrenNodes[0]->isArray){
                    out<<"\tMOV "<<varName<<", CX\n";
                }
                else{
                    out<<"\tMOV [BP], CX\n";
                    out<<"\tPOP BP\n";
                }

                out<<"\tMOV CX, AX\n"; // Restoring CX's value for condition checking
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    out<<"\tJCXZ "<<falseLabel<<":\n";
                    out<<"\tJMP "<<trueLabel<<":\n";
                }
            }

            else if(rule == "factor : variable DECOP"){
                childrenNodes[0]->generateCode(out);
                string varName = childrenNodes[0]->name;
                if(childrenNodes[0]->getGlobalStatus() && !childrenNodes[0]->isArray){
                    out<<"\tMOV CX "<<varName<<endl;
                }
                else{
                    out<<"\tMOV CX, [BP]\n";
                }
                out<<"\tMOV AX, CX\n"; // Storing CX's value for condition checking

                out<<"\tDEC CX\n";

                if(childrenNodes[0]->getGlobalStatus()){
                    out<<"\tMOV "<<varName<<", CX\n";
                }
                else{
                    out<<"\tMOV [BP], CX\n";
                    out<<"\tPOP BP\n";
                }

                out<<"\tMOV CX, AX\n"; // Restoring CX's value for condition checking
                if(isCondition){
                    if(trueLabel == ""){
                        trueLabel = getNewLabel();
                    }
                    if(falseLabel == ""){
                        falseLabel = getNewLabel();
                    }
                    out<<"\tJCXZ "<<falseLabel<<":\n";
                    out<<"\tJMP "<<trueLabel<<":\n";
                }
            }

            else if(rule == "argument_list : arguments"){
                childrenNodes[0]->generateCode(out);
            }

            else if(rule == "arguments : arguments COMMA logic_expression"){
                childrenNodes[0]->generateCode(out);
                childrenNodes[2]->generateCode(out);
                out<<"\tPUSH CX\n"; //pushing func parameter
            }

            else if(rule == "arguments : logic_expression"){
                childrenNodes[0]->generateCode(out);
                out<<"\tPUSH CX\n"; //pushing func parameter
            }
            else{
                for(Node* child : childrenNodes){
                    child->generateCode(out);
                }
            }

        }

};
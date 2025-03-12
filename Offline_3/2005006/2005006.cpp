#include<iostream>
#include<fstream>
#include<sstream>
#include <vector>
using namespace std;


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
    int arraySize = 0;
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

};

class Node{
    int startLine, endLine;
    string type;
    string rule;
    vector<Node*> childrenNodes;
    vector<SymbolInfo*> parameterList; // For functions

    public:
        bool isToken; // For checking if the node is a token eg. ID, COMMA, SEMICOLON etc
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
};
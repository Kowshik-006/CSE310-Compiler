#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

ifstream in("input.txt",ios::in);
ofstream out("output.txt",ios::out);

class SymbolInfo{
    string Name;
    string Type;
    int bucketIndex;
    int blockIndex;
    string scopeTableID;
    public:
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
        SymbolInfo* next;
        SymbolInfo(string name, string type){
            Name = name;
            Type = type;
            next = nullptr;
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
        string getScopeTableID(){
            return scopeTableID;
        }
        void setScopeTableID(string ID){
            scopeTableID = ID;
        }
};

class ScopeTable{
    SymbolInfo** buckets;
    int nBuckets;
    string ID;
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
            symbol = symbol->next;
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
            delete[] buckets;
        }
        
        bool Insert(string name, string type){
            if(LookUp(name) == nullptr){
                int i = (int)sdbm(name);
                int j = 1;
                SymbolInfo* current = buckets[i];
                if(current == nullptr){
                    current = new SymbolInfo(name,type);
                    buckets[i] = current;
                }
                else{
                    SymbolInfo* parent = current;
                    while(current != nullptr){
                        parent = current;
                        current = current->next;
                        j++;
                    }
                    current = new SymbolInfo(name,type);
                    parent->next = current;
                }
                out<<"\tInserted  at position <"<<i+1<<", "<<j<<"> of ScopeTable# "<<ID<<endl; // 1 based indexing
                current->setBucketIndex(i+1);
                current->setBlockIndex(j);
                current->setScopeTableID(ID);
                
                return true;
            }
            out<<"\t'"<<name<<"' already exists in the current ScopeTable# "<<ID<<endl;
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
                    current = current->next;
                }
            }
            return current;
        }

        bool Delete(string name){
            SymbolInfo* current = LookUp(name);
            if(current == nullptr){
                out<<"\tNot found in the current ScopeTable# "<<ID<<endl;
                return false;
            }
            
            int i = current->getBucketIndex() - 1; //1 based to 0 based index;
            int j = current->getBlockIndex() - 1; //1 based to 0 based index;
            if(j==0){
                buckets[i] = current->next;
            }
            else{
                SymbolInfo* parent = buckets[i];
                for(int count=0;count<j-1;count++){
                    parent = parent->next;
                }
                parent->next = current->next;
            }
            out<<"\tDeleted '"<<name<<"' from position <"<<i+1
            <<", "<<j+1<<"> of ScopeTable# "<<ID<<endl;
            decrementBlockIndicesOfNextSymbols(current->next);
            delete current;
            return true;
        }

        void Print(){
            out<<"\tScopeTable# "<<ID<<endl;
            for(int i=0;i<nBuckets;i++){
                out<<"\t"<<i+1;
                SymbolInfo* current = buckets[i];
                while(current != nullptr){
                    out<<" --> ("<<current->getName()<<","<<current->getType()<<")";
                    current = current->next;
                }
                out<<endl;
            }
        }
        string getID(){
            return ID;
        }
        void setID(string id){
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
    public:
        ScopeTable* currentScope;
        SymbolTable(int n){
            nBuckets = n;
            currentScope = new ScopeTable(nBuckets);
            currentScope->setID("1");
            currentScope->parentScope = nullptr;
            out<<"\tScopeTable# "<<currentScope->getID()<<" created\n";
        }
        ~SymbolTable(){
            while(currentScope != nullptr){
                ScopeTable* temp = currentScope->parentScope;
                string ID = currentScope->getID();
                delete currentScope;
                currentScope = temp;
                out<<"\tScopeTable# "<<ID<<" deleted\n";
            }
        }
        void enterScope(){
            string id = currentScope->getID();
            id += "." + to_string(currentScope->incrementCount());
            ScopeTable* newScope = new ScopeTable(nBuckets);
            newScope->setID(id);
            newScope->parentScope = currentScope;
            currentScope = newScope;
            
            out<<"\tScopeTable# "<<id<<" created\n";
        }
        void exitScope(){
            string id = currentScope->getID();
            if(currentScope->parentScope == nullptr){
                out<<"\tScopeTable# "<< id <<" cannot be deleted\n";
            }
            else{
                ScopeTable* temp = currentScope->parentScope;
                delete currentScope;
                currentScope = temp;
                out<<"\tScopeTable# "<< id <<" deleted\n";
            }
        }
        bool Insert(string name,string type){
            return currentScope->Insert(name,type);
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
                    out<<"\t'"<<symbol->getName()<<"' found at position <"<<symbol->getBucketIndex()<<", "<<symbol->getBlockIndex()<<"> of ScopeTable# "<<current->getID()<<endl;
                    return symbol;
                }
            }
            out<<"\t'"<<name<<"' not found in any of the ScopeTables\n";
            return nullptr;
        }
        void printCurrent(){
            currentScope->Print();
        }
        void printAll(){
            ScopeTable* current = currentScope;
            while(current != nullptr){
                current->Print();
                current = current->parentScope;
            }
        }

};
int main(){
    int nBuckets;
    in>>nBuckets;
    int cmdCount = 0;
    string command;
    string commandTokens[10];
    int tokenCount;
    SymbolTable* symbolTable = new SymbolTable(nBuckets);
    getline(in,command);
    while(!in.eof()){
        tokenCount = 0;
        getline(in,command);
        cmdCount++;
        out<<"Cmd "<<cmdCount<<": "<<command<<endl;
        istringstream iss(command);
        while(iss>>commandTokens[tokenCount]){
            tokenCount++;
        }
        if(commandTokens[0] == "I"){
            if(tokenCount != 3){
                out<<"\tWrong number of arugments for the command "<<commandTokens[0]<<endl;
            }
            else{
                symbolTable->Insert(commandTokens[1],commandTokens[2]);
            }
        }
        else if(commandTokens[0] == "L"){
            if(tokenCount != 2){
                out<<"\tWrong number of arugments for the command "<<commandTokens[0]<<endl;
            }
            else{
                symbolTable->Lookup(commandTokens[1]);
            }
        }
        else if(commandTokens[0] == "D"){
            if(tokenCount != 2){
                out<<"\tWrong number of arugments for the command "<<commandTokens[0]<<endl;
            }
            else{
                symbolTable->Remove(commandTokens[1]);
            }
        }
        else if(commandTokens[0] == "P"){
            if(tokenCount != 2){
                out<<"\tWrong number of arugments for the command "<<commandTokens[0]<<endl;
            }
            else{
                if(commandTokens[1] == "A"){
                    symbolTable->printAll();
                }
                else if(commandTokens[1] == "C"){
                    symbolTable->printCurrent();
                }
                else{
                    out<<"\tInvalid argument for the command "<<commandTokens[0]<<endl;
                }
            }
        }
        else if(commandTokens[0] == "S"){
            if(tokenCount != 1){
                out<<"\tWrong number of arugments for the command "<<commandTokens[0]<<endl;
            }
            else{
                symbolTable->enterScope();
            }
        }
        else if(commandTokens[0] == "E"){
            if(tokenCount != 1){
                out<<"\tWrong number of arugments for the command "<<commandTokens[0]<<endl;
            }
            else{
                symbolTable->exitScope();
            }
        }
        else if(commandTokens[0] == "Q"){
            if(tokenCount != 1){
                out<<"\tWrong number of arugments for the command "<<commandTokens[0]<<endl;
            }
            else{
                delete symbolTable;
                break;
            }
        }
        else{
            out<<"\tInvalid command\n";
        }
    }
    in.close();
    out.close();
    return 0;
}

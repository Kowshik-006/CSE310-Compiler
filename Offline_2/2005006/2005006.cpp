#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;


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
            for(int i=0;i<nBuckets;i++){
                SymbolInfo* current = buckets[i];
                SymbolInfo* temp;
                while(current != nullptr){
                    temp = current;
                    current = current->next;
                    delete temp;
                }
            }
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
                    current = current->next;
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
                buckets[i] = current->next;
            }
            else{
                SymbolInfo* parent = buckets[i];
                for(int count=0;count<j-1;count++){
                    parent = parent->next;
                }
                parent->next = current->next;
            }
            // cout<<"\tDeleted '"<<name<<"' from position <"<<i+1
            // <<", "<<j+1<<"> of ScopeTable# "<<ID<<endl;
            decrementBlockIndicesOfNextSymbols(current->next);
            delete current;
            return true;
        }

        void Print(ofstream& out){
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
            // out<<"\tScopeTable# "<<currentScope->getID()<<" created\n";
        }
        ~SymbolTable(){
            while(currentScope != nullptr){
                ScopeTable* temp = currentScope->parentScope;
                string ID = currentScope->getID();
                delete currentScope;
                currentScope = temp;
                // out<<"\tScopeTable# "<<ID<<" deleted\n";
            }
        }
        void enterScope(){
            string id = currentScope->getID();
            id += "." + to_string(currentScope->incrementCount());
            ScopeTable* newScope = new ScopeTable(nBuckets);
            newScope->setID(id);
            newScope->parentScope = currentScope;
            currentScope = newScope;
            
            // cout<<"\tScopeTable# "<<id<<" created\n";
        }
        void exitScope(){
            string id = currentScope->getID();
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
                    // cout<<"\t'"<<symbol->getName()<<"' found at position <"<<symbol->getBucketIndex()<<", "<<symbol->getBlockIndex()<<"> of ScopeTable# "<<current->getID()<<endl;
                    return symbol;
                }
            }
            // out<<"\t'"<<name<<"' not found in any of the ScopeTables\n";
            return nullptr;
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


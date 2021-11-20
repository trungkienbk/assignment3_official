#include "SymbolTable.h"
using namespace std;
void SymbolTable::run(string filename)
{
    ifstream f(filename);
    int cur_level=0;
    while (!f.eof()){
        string ins;
        getline(f, ins);
        if(regex_match(ins,linear)){
           setLinear(ins);
        }
        else if(regex_match(ins,quadratic)){
          setQuadratic(ins);
        }
        else if(regex_match(ins,doublee)){
           setDouble(ins);
        }
        else if(regex_match(ins,ins_vari)) {     ///// HAM INSERT BIEN
           insert_val(ins,cur_level);
        }
        else if (regex_match(ins,ins_func)){ ///// HAM INSERT FUNCTION
            insert_func(ins,cur_level);
        }
        else if (regex_match(ins, ass_val)){ //// HAM ASSIGN VALUE
            assign_value(ins,cur_level);
        }
        else if(regex_match(ins, ass_vari)){ //// HAM ASSIGN VARIABLE
            assign_variable(ins,cur_level);
        }
        else if(regex_match(ins,ass_func)){ //// HAM ASSIGN HAM
            cout<<"ASSIGN HAM OK"<<endl;
        }
        else if(regex_match(ins,look_up)){
            lookup(ins,cur_level);
        }
        else if(regex_match(ins,call)){
            cout<<"CALL OK"<<endl;
        }
        else if(ins == "PRINT"){
            string s="";
            print(s);
            if(s.length() ==0 ) {
                return;
            } else {
                s.pop_back();
            }
            cout<<s<<endl;
        }
        else if(ins == "BEGIN"){
            cur_level++;
        }
        else if(ins =="END"){
            if(cur_level == 0){
                throw UnknownBlock();
            }
            remove(cur_level);
            cur_level--;
        }
        else throw InvalidInstruction(ins);
    }
    f.close();
}
void SymbolTable::setLinear(string ins) {
    int c1_linear= 1;
    int c2_linear= 0;
    int m_linear= 1;
    int index[2]={0,0};
    int j = 0;
    for(int i=0;i<(int)ins.size();++i){
        if(j==2) break;
        if(ins[i]==' '){
            index[j]=i;
            j++;
        }
    }
    m_linear = stoi(ins.substr(index[0]+1,index[1]-index[0]-1));
    c1_linear = stoi(ins.substr(index[1]+1));
    this->setAttribute(c1_linear,c2_linear,m_linear);
    newHash.hp = linearProbing;
}
void SymbolTable::setQuadratic(string ins) {
    int c1_linear;
    int c2_linear;
    int m_linear;
    int index[3]={0,0,0};
    int j = 0;
    for(int i=0;i<(int)ins.size();++i){
        if(j==3) break;
        if(ins[i]==' '){
            index[j]=i;
            j++;
        }
    }
    m_linear = stoi(ins.substr(index[0]+1,index[1]-index[0]-1));
    c1_linear = stoi(ins.substr(index[1]+1,index[2]-index[1]-1));
    c2_linear = stoi(ins.substr(index[2]+1));
    setAttribute(c1_linear,c2_linear,m_linear);
    newHash.hp = quadraticProbing;
    //  cout<<"Check m,c1,c2 " <<m_linear << this->m
    // << " -- "<<c1_linear << this->c1 << " -- "<<c2_linear << this->c2;
}
void SymbolTable::setDouble(string ins) {
    int c1_linear;
    int c2_linear;
    int m_linear;
    int count = 0;
    int index[2]={0,0};
    int j = 0;
    for(int i=0;i<(int)ins.size();++i){
        if(j==2) break;
        if(ins[i]==' '){
            index[j]=i;
            j++;
        }
    }
    m_linear = stoi(ins.substr(index[0]+1,index[1]-index[0]-1));
    c1_linear = stoi(ins.substr(index[1]+1));
    this->setAttribute(c1_linear,c2_linear,m_linear);
    newHash.hp = doubleHashing;
}
void SymbolTable::setAttribute(int c1_linear, int c2_linear, int m_linear) {
    this->c1 = c1_linear;
    this->c2 = c2_linear;
    this->m = m_linear;
    newHash.size = m_linear;
    newHash.status = new int[m];
    newHash.arr = new Symbol[m];
    for (int i = 0; i < newHash.size; i++) {
        newHash.status[i] = 0; // 0 is null, 1 is has element
    }
}

string SymbolTable::encodeName(string name, int cur_level) {
    string res = to_string(cur_level);
    for(int i = 0 ; i < name.length();++i){
        res+=to_string(name[i]-48);
    }
    return res;
}
// 0 la null , 1
void SymbolTable::print(string &s) {
    for (int i = 0; i < newHash.size ; i++) {
        if (newHash.status[i] == 1) {
            //s += to_string(i) + " " + newHash.arr[i].name + "//" + to_string(newHash.arr[i].scope) +";";
            s += to_string(i) + " " + newHash.arr[i].name + "//" + to_string(newHash.arr[i].scope) +newHash.arr[i].type +" ;";
        }
    }
}

Symbol SymbolTable::isContains(Symbol e, int cur_level) {
    string key = encodeName(e.name,cur_level);
    for (int i = 0; i < newHash.size; i++)
    {
        int k = newHash.hp(key, i, c1, c2, m);
        if (newHash.arr[k].encode == key && newHash.status[k] == 1)
            return newHash.arr[k];
    }
    return Symbol("null",-1);
}

void SymbolTable::insert_val(string ins, int cur_level) {
    string name;
    int index = ins.find(" ");
    name = ins.substr(index+1);
    Symbol e(name,cur_level);
    e.encode = encodeName(name,cur_level);
    Symbol check = isContains(e,cur_level);
    //if(check.name != "null") throw Redeclared(ins);
    if(check.name != "null") throw Redeclared(name);
    //  cout<<" thuoc tinh  : c1->"<<c1<<" c2->"<<c2<<" m(size) ->"<<m<<endl;
    for(int i = 0 ; i < newHash.size ; ++i){
        int k = newHash.hp(e.encode,i,c1,c2,m);
        // cout<<"key " <<k<< " -> count : ";
        if( newHash.status[k] == 0 ){
            newHash.arr[k] = e ;
            newHash.status[k] = 1;
            newHash.arr[k].index=k;
            cout<<i<<endl;
            return;
        }
    }
    throw Overflow(ins);
}

void SymbolTable::lookup(string ins, int cur_level) {
    string name;
    int index = ins.find(" ");
    name = ins.substr(index+1);
    Symbol check;
    while(cur_level >= 0) {
        Symbol e(name,cur_level);
        e.encode = encodeName(name,cur_level);
        check = isContains(e,cur_level);
        if(check.name !="null") break;
        cur_level--;
    }
    if(check.name =="null") throw Undeclared(name);
    cout<<check.index<<endl;
}

void SymbolTable::remove(int cur_level) {
    for(int i = 0; i < newHash.size ;++i){
        if(newHash.arr[i].scope !=0  && newHash.arr[i].scope == cur_level && newHash.status[i] ==1){
            newHash.status[i] = 0;
        }
    }
}

void SymbolTable::insert_func(string ins, int cur_level) {
    string name,argList;
    string argList_tmp = "n";
    int number_argu = 0 ;
    int index[2]={0,0};
    int j = 0;
    for(int i=0;i<(int)ins.size();++i){
        if(j==2) break;
        if(ins[i]==' '){
            index[j]=i;
            j++;
        }
    }
    name = ins.substr(index[0]+1,index[1]-index[0]-1);
    number_argu = stoi(ins.substr(index[1]+1));
    while(number_argu > 0){
        argList_tmp+="n";
        number_argu--;
    }
    Symbol e(name,cur_level);
    e.encode = encodeName(name,cur_level);
    Symbol check = isContains(e,cur_level);
    if(check.name != "null") throw Redeclared(name);
    if(cur_level > 0) throw InvalidDeclaration(name);
    //if(check.name != "null") throw Redeclared(ins);
   // if(cur_level > 0) throw InvalidDeclaration(ins);
    //  cout<<" thuoc tinh  : c1->"<<c1<<" c2->"<<c2<<" m(size) ->"<<m<<endl;
    for(int i = 0 ; i < newHash.size ; ++i){
        int k = newHash.hp(e.encode,i,c1,c2,m);
        // cout<<"key " <<k<< " -> count : ";
        if( newHash.status[k] == 0 ){
            newHash.arr[k] = e ;
            newHash.status[k] = 1;
            newHash.arr[k].index=k;
            newHash.arr[k].argList = argList_tmp;
            cout<<i<<endl;
           // cout<<"Test count N of "+newHash.arr[k].name<<" "<<newHash.arr[k].argList<<endl;
            return;
        }
    }
    throw Overflow(ins);
}

void SymbolTable::call_func(string ins) {

}

void SymbolTable::assign_value(string ins, int cur_level) {
    string name,valu,value_type;
    int idx = 0;
    int num_step = 0;
    int index[2]={0,0};
    int j = 0;
    for(int i=0;i<(int)ins.size();++i){
        if(j==2) break;
        if(ins[i]==' '){
            index[j]=i;
            j++;
        }
    }
    name = ins.substr(index[0]+1,index[1]-index[0]-1);
    valu = ins.substr(index[1]+1);
    if(valu[0]=='\'') value_type = "string";
    else value_type = "number";
    Symbol e(name,cur_level);
    e.encode = encodeName(name,cur_level);
    Symbol check = search(name,cur_level,idx,num_step);
    if(check.name == "null") throw Undeclared(name);
    if(check.type == "void") throw TypeMismatch(ins);
    if(check.argList != "") throw TypeMismatch(ins);
    if(check.type == ""){
        newHash.arr[idx].type = value_type;
        cout<<num_step<<endl;
        return;
    } else {
        if(check.type != value_type){
            throw TypeMismatch(ins);
        }
        cout<<num_step<<endl;
        return;
    }
}
Symbol SymbolTable::search(string name, int cur_level, int& idx, int& num_step) {
    while(cur_level >= 0){
      //  num_step= 0;
        string key = encodeName(name,cur_level);
        for (int i = 0; i < newHash.size; i++){
            int k = newHash.hp(key, i, c1, c2, m);
            if (newHash.arr[k].encode == key && newHash.status[k] == 1){
                idx = k;
                return newHash.arr[k];
            }
            num_step++;
        }
        cur_level--;
    }
    return Symbol("null",-1);
}

void SymbolTable::assign_variable(string ins,int cur_level) {
    string id,value;
    int num_step = 0;
    int idx_val,idx_id;
    int index[2]={0,0};
    int j = 0;
    for(int i=0;i<(int)ins.size();++i){
        if(j==2) break;
        if(ins[i]==' '){
            index[j]=i;
            j++;
        }
    }
    id = ins.substr(index[0]+1,index[1]-index[0]-1);
    value = ins.substr(index[1]+1);
    Symbol check_val = search(value,cur_level,idx_val,num_step);
    if(check_val.name == "null") throw Undeclared(value);
    if(check_val.argList != "") throw TypeMismatch(ins);
    Symbol check_id = search(id,cur_level,idx_id,num_step);
    if(check_id.name == "null") throw Undeclared(id);
    if(check_id.argList != "") throw TypeMismatch(ins);
    if(check_id.type == "" && check_val.type == "") throw TypeCannotBeInfered(ins);
    if(check_id.type != "" && check_val.type != ""){
        if(check_id.type != check_val.type) throw TypeMismatch(ins);
        else{
            cout<<num_step<<endl;
        }
        return;
    }
    if(check_id.type != "" && check_val.type == ""){
        newHash.arr[idx_val].type = newHash.arr[idx_id].type;
        cout<<num_step<<endl;
        return;
    }
    if(check_id.type == "" && check_val.type != ""){
        newHash.arr[idx_id].type = newHash.arr[idx_val].type;
        cout<<num_step<<endl;
        return;
    }
}

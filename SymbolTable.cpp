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
           SetLinear(ins);
        }
        else if(regex_match(ins,quadratic)){
          SetQuadratic(ins);
        }
        else if(regex_match(ins,doublee)){
           SetDouble(ins);
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
            assign_function(ins,cur_level);
        }
        else if(regex_match(ins,look_up)){
            lookup(ins,cur_level);
        }
        else if(regex_match(ins,call)){
            call_func(ins,cur_level);
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
void SymbolTable::SetLinear(string ins) {
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
    this->SetAttribute(c1_linear,c2_linear,m_linear);
    newHash.hp = linearProbing;
}
void SymbolTable::SetQuadratic(string ins) {
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
    SetAttribute(c1_linear,c2_linear,m_linear);
    newHash.hp = quadraticProbing;
    //  cout<<"Check m,c1,c2 " <<m_linear << this->m
    // << " -- "<<c1_linear << this->c1 << " -- "<<c2_linear << this->c2;
}
void SymbolTable::SetDouble(string ins) {
    int c1_linear;
    int c2_linear =1;
    int m_linear;
   // int count = 0;
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
    this->SetAttribute(c1_linear,c2_linear,m_linear);
    newHash.hp = doubleHashing;
}
void SymbolTable::SetAttribute(int c1_linear, int c2_linear, int m_linear) {
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
    for(int i = 0 ; i <(int) name.length();++i){
        res+=to_string(name[i]-48);
    }
    return res;
}
// 0 la null , 1
void SymbolTable::print(string &s) {
    for (int i = 0; i < newHash.size ; i++) {
        if (newHash.status[i] == 1) {
            s += to_string(i) + " " + newHash.arr[i].name + "//" + to_string(newHash.arr[i].scope) +";";
            //s += to_string(i) + " " + newHash.arr[i].name + "//" + to_string(newHash.arr[i].scope) +"_"+newHash.arr[i].type +"_"+newHash.arr[i].argList+" ;";
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
    if(isValidId(name) == false) throw InvalidInstruction(ins); // Na
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
    if(isValidId(name) == false) throw InvalidInstruction(ins); // Na
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

void SymbolTable::call_func(string ins,int cur_level) {
    if(isKeyword(ins)) throw InvalidInstruction(ins);
    int idx = 0;
    int num_step = 0;
    int idx_real;
    string value_func,func_name,argu;
    int space = ins.find(' ');
    value_func = ins.substr(space+1);
    check_function(ins,value_func, cur_level,idx,num_step,idx_real);
    if(newHash.arr[idx_real].type != ""){
        throw TypeMismatch(ins);
    } else {
        newHash.arr[idx_real].type = "void";
    }
    cout<<num_step<<endl;
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
    if(isValidId(name) == false) throw InvalidInstruction(ins);
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
    int tmp = num_step;
    while(cur_level >= 0){
        num_step= tmp;
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
    if(isValidId(id) == false) throw InvalidInstruction(ins); // Name la key word
    if(isValidId(value) == false) throw InvalidInstruction(ins);
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

void SymbolTable::check_function(string ins,string value_func,int cur_level,int idx,int& num_step,int& idx_func_real) {
    string func_name,argu;
    int count_point = 0;
    int index_par[40];
    int find_parless = (int)value_func.find('(');
    func_name = value_func.substr(0,find_parless); //// Name of function
    argu = value_func.substr(find_parless); //// List argument; ex: (1,'234)
    Symbol temp_func = search(func_name,cur_level,idx,num_step);
    idx_func_real = idx;
    if(temp_func.name == "null"){
        throw Undeclared(func_name);
    }
    if(temp_func.type == "void") throw TypeMismatch(ins); ///////////////////// them day ne
    if(temp_func.argList == "") throw TypeMismatch(ins);
    if(temp_func.type != ""){ //// da co tye, lam nhu asm2
        if(temp_func.argList.length() == 1){ //// No argument
            //// CHECK DIEU KIEN TYPE CUA HAM RONG
            if(argu.length() > 2 ) throw TypeMismatch(ins);
        }
        else
        {
            for(char i : argu){
                if(i == ','){
                    count_point++;
                }
            }
            if(count_point + 2 != (int) temp_func.argList.length()) throw TypeMismatch(ins);
            count_point = 0;
            for(int i = 0 ;i<(int) argu.length();++i){
                if(argu[i] == '(' ||argu[i] == ')' ||argu[i] == ','){
                    index_par[count_point] = i;
                    count_point++;
                }
            }
            int k = 0;
            for(int i = 0 ; i< count_point-1;++i){
                string temp = argu.substr(index_par[i]+1,index_par[i+1]-index_par[i]-1);
                if(temp[0]>='0'  &&temp[0]<='9'){
                    if(temp_func.argList[k] != 'n') throw TypeMismatch(ins);
                }
                else if(temp[0]=='\''){
                    if(temp_func.argList[k] != 's') throw TypeMismatch(ins);
                }
                else {
                    Symbol temp_argu = search(temp,cur_level,idx,num_step);
                    if(temp_argu.name == "null") throw Undeclared(temp); /// ko thay ten
                    if(temp_argu.argList !="") throw TypeMismatch(ins);/// truyen ham
                    if(temp_argu.type =="number" && temp_func.argList[k]=='s') throw TypeMismatch(ins);
                    if(temp_argu.type =="string" && temp_func.argList[k]=='n') throw TypeMismatch(ins);
                   // if(temp_argu.type =="")  throw TypeCannotBeInfered(ins);        THEM DAY NE
                    if(temp_argu.type =="") {
                       if(temp_func.argList[k] == 'n'){
                           newHash.arr[idx].type = "number";
                       } else {
                           newHash.arr[idx].type = "string";
                       }
                    }
                }
                ++k;
            }
            ////////
        }
    }
    else { //// Chua co type
        if(temp_func.argList.length() == 1){ //// No argument
            //// CHECK DIEU KIEN TYPE CUA HAM RONG
            if(argu.length() > 2 ) throw TypeMismatch(ins);
        }
        else {
            for(char i : argu){
                if(i == ','){
                    count_point++;
                }
            }
            if(count_point + 2 != (int) temp_func.argList.length()) throw TypeMismatch(ins);
            count_point = 0;
            for(int i = 0 ;i<(int) argu.length();++i){
                if(argu[i] == '(' ||argu[i] == ')' ||argu[i] == ','){
                    index_par[count_point] = i;
                    count_point++;
                }
            }
            int k = 0;
            for(int i = 0 ; i< count_point-1;++i){
                string temp = argu.substr(index_par[i]+1,index_par[i+1]-index_par[i]-1);
                if(temp[0]>='0'  &&temp[0]<='9'){
                    newHash.arr[idx].argList[k] = 'n';
                }
                else if(temp[0]=='\''){
                    newHash.arr[idx].argList[k]= 's';
                }
                else {
                    Symbol temp_argu = search(temp,cur_level,idx,num_step);

                    if(temp_argu.name == "null") throw Undeclared(temp); /// ko thay ten
                    if(temp_argu.argList !="") throw TypeMismatch(ins);/// truyen ham
                    if(temp_argu.type =="")  throw TypeCannotBeInfered(ins);
                    else {
                        if(temp_argu.type == "string"){
                            newHash.arr[idx].argList[k]= 's';
                        } else newHash.arr[idx].argList[k] = 'n';
                    }
                }++k;
            }
        }
    }
    return;
}
/*
 void SymbolTable::check_function(string ins,string value_func,int cur_level,int idx,int& num_step,int& idx_func_real) {
    string func_name,argu;
    int count_point = 0;
    int index_par[40];
    int find_parless = (int)value_func.find('(');
    func_name = value_func.substr(0,find_parless); //// Name of function
    argu = value_func.substr(find_parless); //// List argument; ex: (1,'234)
    Symbol temp_func = search(func_name,cur_level,idx,num_step);
    idx_func_real = idx;
    if(temp_func.name == "null"){
        throw Undeclared(func_name);
    }
    if(temp_func.argList == "") throw TypeMismatch(ins);
    if(temp_func.type != ""){ //// da co tye, lam nhu asm2
        if(temp_func.argList.length() == 1){ //// No argument
            //// CHECK DIEU KIEN TYPE CUA HAM RONG
            if(argu.length() > 2 ) throw TypeMismatch(ins);
        }
        else
        {
            for(char i : argu){
                if(i == ','){
                    count_point++;
                }
            }
            if(count_point + 2 != (int) temp_func.argList.length()) throw TypeMismatch(ins);
            count_point = 0;
            for(int i = 0 ;i<(int) argu.length();++i){
                if(argu[i] == '(' ||argu[i] == ')' ||argu[i] == ','){
                    index_par[count_point] = i;
                    count_point++;
                }
            }
            int k = 0;
            for(int i = 0 ; i< count_point-1;++i){
                string temp = argu.substr(index_par[i]+1,index_par[i+1]-index_par[i]-1);
                if(temp[0]>='0'  &&temp[0]<='9'){
                    if(temp_func.argList[k] != 'n') throw TypeMismatch(ins);
                }
                else if(temp[0]=='\''){
                    if(temp_func.argList[k] != 's') throw TypeMismatch(ins);
                }
                else {
                    Symbol temp_argu = search(temp,cur_level,idx,num_step);
                    if(temp_argu.name == "null") throw Undeclared(temp); /// ko thay ten
                    if(temp_argu.argList !="") throw TypeMismatch(ins);/// truyen ham
                    if(temp_argu.type =="number" && temp_func.argList[k]=='s') throw TypeMismatch(ins);
                    if(temp_argu.type =="string" && temp_func.argList[k]=='n') throw TypeMismatch(ins);
                    if(temp_argu.type =="")  throw TypeCannotBeInfered(ins);
                }
                ++k;
            }
            ////////
        }
    }
    else {
        if(temp_func.argList.length() == 1){ //// No argument
            //// CHECK DIEU KIEN TYPE CUA HAM RONG
            if(argu.length() > 2 ) throw TypeMismatch(ins);
        }
        else {
            for(char i : argu){
                if(i == ','){
                    count_point++;
                }
            }
            if(count_point + 2 != (int) temp_func.argList.length()) throw TypeMismatch(ins);
            count_point = 0;
            for(int i = 0 ;i<(int) argu.length();++i){
                if(argu[i] == '(' ||argu[i] == ')' ||argu[i] == ','){
                    index_par[count_point] = i;
                    count_point++;
                }
            }
            int k = 0;
            for(int i = 0 ; i< count_point-1;++i){
                string temp = argu.substr(index_par[i]+1,index_par[i+1]-index_par[i]-1);
                if(temp[0]>='0'  &&temp[0]<='9'){
                    newHash.arr[idx].argList[k] = 'n';
                }
                else if(temp[0]=='\''){
                    newHash.arr[idx].argList[k]= 's';
                }
                else {
                    Symbol temp_argu = search(temp,cur_level,idx,num_step);
                    if(temp_argu.name == "null") throw Undeclared(temp); /// ko thay ten
                    if(temp_argu.argList !="") throw TypeMismatch(ins);/// truyen ham
                    if(temp_argu.type =="")  throw TypeCannotBeInfered(ins);
                    else {
                        if(temp_argu.type == "string"){
                            newHash.arr[idx].argList[k]= 's';
                        } else newHash.arr[idx].argList[k] = 'n';
                    }
                }++k;
            }
        }
    }
    return;
}
 */

void SymbolTable::assign_function(string ins, int cur_level) {
    if(isKeyword(ins)) throw InvalidInstruction(ins);
    string value_func,id;
    int num_step = 0;
    int idx_var = 0;
    int idx_func_fake = 0;
    int idx_func = 0;
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
    value_func  = ins.substr(index[1]+1);
  //  Symbol tmp = search(id,cur_level,idx_func,num_step);
   // cout<<"sau khi goi cccc function "<<num_step<<endl;
    check_function(ins,value_func,cur_level,idx_func_fake,num_step,idx_func);
   // cout<<"sau khi goi check function "<<num_step<<endl;
    Symbol check_id = search(id,cur_level,idx_var,num_step);
    //    cout<<"sau khi goi id "<<num_step<<endl;
   // cout<<"indexx ne " << "func : "<<idx_func<<"_"<<newHash.arr[idx_func].name<<"      var : "<<idx_var<<"_"<<newHash.arr[idx_var].name<<endl;
    if(check_id.name == "null") throw Undeclared(id);
    if(check_id.argList !="") throw TypeMismatch(ins);

    if(newHash.arr[idx_var].type == "" && newHash.arr[idx_func].type == ""){
        throw TypeCannotBeInfered(ins);
    }
    if(newHash.arr[idx_var].type == "" && newHash.arr[idx_func].type == "void"){
        throw TypeMismatch(ins);
    }
    if(newHash.arr[idx_var].type != "" && newHash.arr[idx_func].type != "" ){
        if(newHash.arr[idx_var].type !=newHash.arr[idx_func].type )
        throw TypeMismatch(ins);
    }

    if(newHash.arr[idx_var].type == "" && newHash.arr[idx_func].type != ""){
        newHash.arr[idx_var].type=newHash.arr[idx_func].type;
    }
    if(newHash.arr[idx_var].type != "" && newHash.arr[idx_func].type == ""){
        newHash.arr[idx_func].type=newHash.arr[idx_var].type;
    }
    cout<<num_step<<endl;
}

bool SymbolTable::isValidId(string id) {
    if( id =="string" || id =="number") return false;
    return true;
}

bool SymbolTable::isKeyword(const string &ins) {
    string id, valu;
    stringstream ss(ins);
    ss >> id >> id;
    getline(ss,valu);
    valu = valu.substr(1);
    if(!isValidId(id)) return true;
    if(valu[valu.size()-1]==')') {
        size_t posi_S = 0;
        size_t posi_E = valu.find('(');

        string funcIden = valu.substr(posi_S, posi_E);
        if(!isValidId(funcIden)) return true;

        if(valu[valu.size()-2] != '(') {
            posi_S = posi_E+1;
            posi_E = valu.find(',', posi_S);

            while(posi_E != string::npos) {
                string paraName = valu.substr(posi_S, posi_E-posi_S);
                if(!isValidId(paraName)) return true;
                posi_S = posi_E+1;
                posi_E = valu.find(',', posi_S);
            }
            string argu_name = valu.substr(posi_S, valu.size() - posi_S - 1);
            if(!isValidId(argu_name)) return true;
        }

    } else {
        if(!isValidId(valu)) return true;
    }
    return false;
}

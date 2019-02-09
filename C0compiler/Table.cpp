#include<iostream>
#include<string>
#include<cstdlib>
#include"Global.h"

using namespace std;
//符号表极其指针
Symbol_ele sym_table[MAX_OF_SYMTABLE];
int ptrOFsymtable=1;
//函数符号表及其指针
Func_ele func_table[MAX_OF_FUNCTABLE];
int ptrOFfunctable=1;
//字符串表及其指针
string str_table[MAX_OF_STRTABLE];//字符串表
int ptrOFstrtable=1;
//记录当前函数的符号表头指针，便于之后的释放及各种操作
int nowfunc_index=1;

int search_symtable(string _name,int _begin,int _end){
    int i;
    for(i=_end-1;i>=_begin;i--){
        if(sym_table[i].name==_name){
            return i;
        }
    }
    return 0;
}
int search_functable(string _name){
    int i;
    for(i=ptrOFfunctable-1;i>=1;i--){
        if(func_table[i].name==_name){
            return i;
        }
    }
    return 0;
}
void enter_symtable(string _name, int _kind, int _type, int _value){
    int i=ptrOFsymtable;
    sym_table[i].name=_name;
    sym_table[i].kind=_kind;
    sym_table[i].type=_type;
    sym_table[i].value=_value;
    ptrOFsymtable++;
    if(ptrOFsymtable==MAX_OF_SYMTABLE){
        error(TABLEOVERFLOW);
        cout<<"Compile error, not finished! Total: "<<err_num<<" errors!"<<endl;
        exit(0);
    }
}
void enter_functable(string _name, int _addr, int _parcount, int _varcount){
    int i=ptrOFfunctable;
    func_table[i].name=_name;
    func_table[i].addr=_addr;
    func_table[i].parcount=_parcount;
    func_table[i].varcount=_varcount;
    ptrOFfunctable++;
    if(ptrOFfunctable==MAX_OF_FUNCTABLE){
        error(TABLEOVERFLOW);
        cout<<"Compile error, not finished! Total: "<<err_num<<" errors!"<<endl;
        exit(0);
    }
}
int enter_strtable(string _str){
    int i;
    for(i=1;i<ptrOFstrtable;i++){
        if(str_table[i]==_str) return i;
    }
    str_table[ptrOFstrtable++]=sym_str;
    return (ptrOFstrtable-1);
}
void popsym_table(){
    int i;
    int par=sym_table[nowfunc_index].value;
    for(i=nowfunc_index+1;i<=nowfunc_index+par;i++){
        sym_table[i].name=" ";
    }
    ptrOFsymtable=nowfunc_index+par+1;
}
void out_table(){
    int i;
    for(i=1;i<ptrOFsymtable;i++){
        cout<<sym_table[i].name<<" "<<sym_table[i].kind<<" "<<sym_table[i].type<<" "<<sym_table[i].value<<endl;
    }
    cout<<endl<<"Here's function table!"<<endl<<endl;
    for(i=1;i<ptrOFfunctable;i++){
        cout<<func_table[i].name<<" "<<func_table[i].addr<<" "<<func_table[i].parcount<<" "<<func_table[i].varcount<<endl;
    }
    cout<<endl<<"Here's string table!"<<endl<<endl;
    for(i=1;i<ptrOFstrtable;i++){
        cout<<str_table[i]<<endl;
    }
}

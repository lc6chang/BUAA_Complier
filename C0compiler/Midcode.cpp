#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include"Global.h"

using namespace std;

Midcode_ele midcode_table[MAX_OF_MIDCODE];
int ptrOFmidcode=0;

int tempvar_num=0;
int label_num=0;

void emit(string _op,string _var1,string _var2,string _var3){
    int i;
    i=ptrOFmidcode;
    midcode_table[i].op=_op;
    midcode_table[i].var1=_var1;
    midcode_table[i].var2=_var2;
    midcode_table[i].var3=_var3;
    ptrOFmidcode++;
}
string nextempvar(){
    stringstream newstr;
    newstr<<"#_t"<<tempvar_num++;
    return newstr.str();
}
string nextlabel(){
    stringstream newstr;
    newstr<<"_Label"<<label_num++<<"_";
    return newstr.str();
}
void out_midcode(){
    int i;
    for(i=0;i<ptrOFmidcode;i++){
        cout<<midcode_table[i].op<<" , "<<midcode_table[i].var1<<" , ";
        cout<<midcode_table[i].var2<<" , "<<midcode_table[i].var3<<endl;
    }
}
void outMidcode(string file){
    ofstream outmid;
    outmid.open(file.data());
    int i;
    for(i=0;i<ptrOFmidcode;i++){
        outmid<<midcode_table[i].op<<" , "<<midcode_table[i].var1<<" , ";
        outmid<<midcode_table[i].var2<<" , "<<midcode_table[i].var3<<endl;
    }
    outmid.close();
}

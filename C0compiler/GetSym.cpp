#include<iostream>
#include<string>
#include<cstdlib>
#include"Global.h"

using namespace std;

string keywords[13]={
    "const","int","char","void","main",
    "if","while","switch","case",
    "default","scanf","printf","return"
    };
string str_type[37]={
    "CONSTSYM","INTSYM","CHARSYM","VOIDSYM","MAINSYM",
    "IFSYM","WHILESYM","SWITCHSYM","CASESYM",
    "DEFAULTSYM","SCANFSYM","PRINTFSYM","RETURNSYM",
    "PLUSYM","MINUSYM","TIMESYM","DIVSYM",
    "LESSYM","LEQSYM","GTRSYM","GEQSYM","ASGSYM","EQLSYM","NEQLSYM",
    "COMMA","COLON","SEMICOLON",
    "LPARENT","RPARENT","LBRACKET","RBRACKET","LBRACE","RBRACE",
    "INTCON","CHARCON","STRCON","IDENTIFIER"
};
string str_sps[20]={
    "+","-","*","/","<","<=",">",">=","=","==","!=",
    ",",":",";",
    "(",")","[","]","{","}"
};
int sym_count=0;//单词计数器
int line_count=1;//行号计数器
int column_count=0;//列号计数器
int sym_type=0;//读到单词的类型
int sym_num=0;//读到数字的值
char sym_char='\0';//读到字符的值
string sym_str="";//读到字符串的值
string sym_id="";

char ctemp='\0';
string sym_Token="";//读到的字符

int isletter(){
    if(ctemp>='a'&&ctemp<='z') return 1;
    if(ctemp>='A'&&ctemp<='Z') return 1;
    if(ctemp=='_') return 1;
    return 0;
}
int isnumber(){
    if(ctemp>='0'&&ctemp<='9') return 1;
    return 0;
}
void nextchar(){
    if(ptrOFincode==incode.length()) {
        ctemp=-1;
        return;
    }
    ctemp=incode[ptrOFincode++];
    column_count++;
}
int findkey(){//查找关键字
    int i;
    for(i=0;i<13;i++){
        if(keywords[i]==sym_Token){
            return i;
        }
    }
    return -1;
}
void output(){//输出
    /*sym_count++;
    if(sym_type==IDENTIFIER) cout<<sym_count<<"  "<<"IDENTIFIER  "<<sym_id<<endl;
    else if(sym_type==INTCON) cout<<sym_count<<"  "<<"INTCON  "<<sym_num<<endl;
    else if(sym_type==CHARCON) cout<<sym_count<<"  "<<"CHARCON  "<<sym_char<<endl;
    else if(sym_type==STRCON) cout<<sym_count<<"  "<<"STRCON  "<<sym_str<<endl;
    else if(sym_type>=0&&sym_type<13) cout<<sym_count<<"  "<<str_type[sym_type]<<"  "<<sym_id<<endl;
    else cout<<sym_count<<"  "<<str_type[sym_type]<<"  "<<str_sps[sym_type-13]<<endl;*/
}
void outsym(){//输出symbol
    if(sym_type==INTCON) cout<<sym_num;
    else if(sym_type==CHARCON) cout<<sym_char;
    else if(sym_type==STRCON) cout<<sym_str;
    else if(sym_type>=13&&sym_type<=32) cout<<str_sps[sym_type-13];
    else cout<<sym_id;
}
void insymbol(){
    int i;
    sym_Token="";
    //首先将前面的空格、TAB、换行过滤掉
    while(1){

    while(ctemp==' '||ctemp=='\t'||ctemp=='\n'){
        if(ctemp=='\n'){
            line_count++;
            column_count=0;
        }
        nextchar();
    }
    if(ctemp==-1){
        cout<<"Fatal! Unexpected reach the end of the input code file!"<<endl;
        cout<<"Compile error, not finished! Total: "<<err_num<<" errors"<<endl;
        exit(0);
    }
    else if(isletter()){//标识符读取
        sym_Token+=ctemp;
        nextchar();
        while(isletter()||isnumber()){
            sym_Token+=ctemp;
            nextchar();
        }
        i=findkey();
        if(i!=-1){
            sym_type=i;
            sym_id=sym_Token;
            output();
        }
        else{
            sym_type=IDENTIFIER;
            sym_id=sym_Token;
            output();
        }
    }
    else if(isnumber()){//无符号数字读取
        int count0=0;
        int flag0=1;
        sym_num=ctemp-'0';
        if(ctemp=='0') count0++;
        else flag0=0;
        nextchar();
        while(isnumber()){
            if(flag0&&ctemp=='0') count0++;
            else flag0=0;
            sym_num=sym_num*10+ctemp-'0';
            nextchar();
        }
        if(sym_num!=0&&count0!=0) error(LEADINGZERO);//cout<<"wrong leading zero!"<<endl;
        else if(sym_num==0&&count0!=1) error(LEADINGZERO);
        sym_type=INTCON;
        output();
    }
    else if(ctemp=='\''){
        nextchar();
        if(ctemp=='+'||ctemp=='-'||ctemp=='*'||ctemp=='/'||isletter()||isnumber()){
            sym_char=ctemp;
            nextchar();
        }
        else{
            //cout<<"error char const!"<<endl;
            error(ERRORCHAR);
            sym_char='a';//容错处理，先把它当做是正确的
        }
        if(ctemp!='\'') error(NEEDQUOTA);//cout<<"here needs '"<<endl;
        else nextchar();
        sym_type=CHARCON;
        output();
    }
    else if(ctemp=='\"'){
        nextchar();
        while(ctemp>=' '&&ctemp<='~'&&ctemp!='\"'){
            if(ctemp=='\\') sym_Token=sym_Token+'\\';
            sym_Token=sym_Token+ctemp;
            nextchar();
        }
        if(ctemp!='\"') error(NEEDOUBLEQUOTA);//cout<<"here needs \" "<<endl;
        else nextchar();
        sym_type=STRCON;
        sym_str=sym_Token;
        output();
    }
    else{
        switch(ctemp){
            case '+':{
                sym_type=PLUSYM;
                nextchar();
                output();
                break;
            }
            case '-':{
                sym_type=MINUSYM;
                nextchar();
                output();
                break;
            }
            case '*':{
                sym_type=TIMESYM;
                nextchar();
                output();
                break;
            }
            case '/':{
                sym_type=DIVSYM;
                nextchar();
                output();
                break;
            }
            case '<':{
                nextchar();
                if(ctemp=='='){
                    sym_type=LEQSYM;
                    nextchar();
                    output();
                    break;
                }
                sym_type=LESSYM;
                output();
                break;
            }
            case '>':{
                nextchar();
                if(ctemp=='='){
                    sym_type=GEQSYM;
                    nextchar();
                    output();
                    break;
                }
                sym_type=GTRSYM;
                output();
                break;
            }
            case '=':{
                nextchar();
                if(ctemp=='='){
                    sym_type=EQLSYM;
                    nextchar();
                    output();
                    break;
                }
                sym_type=ASGSYM;
                output();
                break;
            }
            case '!':{
                nextchar();
                if(ctemp!='=') error(NEEDEQUAL);//cout<<"here needs = !"<<endl;
                else nextchar();
                sym_type=NEQLSYM;
                output();
                break;
            }
            case ',':{
                sym_type=COMMA;
                nextchar();
                output();
                break;
            }
            case ':':{
                sym_type=COLON;
                nextchar();
                output();
                break;
            }
            case ';':{
                sym_type=SEMICOLON;
                nextchar();
                output();
                break;
            }
            case '(':{
                sym_type=LPARENT;
                nextchar();
                output();
                break;
            }
            case ')':{
                sym_type=RPARENT;
                nextchar();
                output();
                break;
            }
            case '[':{
                sym_type=LBRACKET;
                nextchar();
                output();
                break;
            }
            case ']':{
                sym_type=RBRACKET;
                nextchar();
                output();
                break;
            }
            case '{':{
                sym_type=LBRACE;
                nextchar();
                output();
                break;
            }
            case '}':{
                sym_type=RBRACE;
                nextchar();
                output();
                break;
            }
            default:{
                error(ERRORSYMBOL);//cout<<"error in type!"<<endl;
                nextchar();
                continue;
            }
        }
    }
    break;
    }//end of while(1)
}



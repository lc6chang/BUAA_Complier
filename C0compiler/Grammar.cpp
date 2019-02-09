#include<iostream>
#include<sstream>
#include"Global.h"
/////////////////////////////////////////////////
int type_reg=0;//寄存类型、指针和行号，便于返回
int inptr_reg=0;
char ctemp_reg='\0';
/////////////////////////////////////////////////
string str2mid[3]={"int","char","void"};
////////////////////////////////////////////////
int ifmain=0;//判断是否有主函数
int ifreturn=0;//记录是否有return
///////////////////////////////////////////////
int par_num=0;//参数个数
int var_num=0;//变量/临时变量个数
int now_functype=0;//存储当前函数的返回值类型
//////////////////////////////////////////////
string last_var="";//存储上一次调用结果的变量名
int last_type=0;//存储上一次运算结果的类型

string int2string(int n){
    stringstream newstr;
    newstr<<n;
    return newstr.str();
}
/*int read_int(){//读整数，处理前面的+/-号
    int flag=1;
    int flag2=0;//标记是有符号整数还是无符号整数
    if(sym_type==PLUSYM){
        insymbol();
        flag2=1;
    }
    else if(sym_type==MINUSYM){
        flag=-1;
        insymbol();
        flag2=1;
    }
    if(sym_type==INTCON){
        sym_num=sym_num*flag;
        insymbol();
    }
    else{
        cout<<"need integer here!"<<endl;
        //skip();
        return -1;
    }
    if(flag2){
        return 1;
    }
    return 0;
    //cout<<line_count<<" is integer!"<<endl;
}*/

void con_def(){//常量定义
    int signflag=1;
    string name="";//记录当前常量的name
    int value=0;//记录当前常量的value
    if(sym_type==INTSYM){//如果是整数
        do{
            insymbol();
            if(sym_type==IDENTIFIER){
                name=sym_id;//记下名字
                insymbol();
                if(sym_type==ASGSYM) insymbol();
                else error(NEEDEQUAL);//cout<<"here needs ="<<endl;
                signflag=1;
                if(sym_type==PLUSYM) insymbol();
                else if(sym_type==MINUSYM){
                    signflag=-1;
                    insymbol();
                }
                if(sym_type==INTCON){
                    value=sym_num*signflag;
                    insymbol();
                }
                else{
                    //cout<<"here needs int const!"<<endl;
                    error(NEEDINTCON);
                    value=0;
                }
                //登录符号表
                if(search_symtable(name,nowfunc_index,ptrOFsymtable)){
                    error(REDECLARE);
                }
                else{
                    enter_symtable(name,K_CONST,T_INT,value);
                }
            }
            else{
                //cout<<"here needs identifier!"<<endl;
                error(NEEDIDENTIFIER);
                skip(0);//skip to , or ;
            }
        }while(sym_type==COMMA);
    }
    else if(sym_type==CHARSYM){//如果是字符
        do{
            insymbol();
            if(sym_type==IDENTIFIER){
                name=sym_id;
                insymbol();
                if(sym_type==ASGSYM) insymbol();
                else error(NEEDEQUAL);//cout<<"here needs =!"<<endl;
                if(sym_type==CHARCON){
                    insymbol();
                    value=(int)sym_char;
                }
                else{
                    error(NEEDCHARCON);//cout<<"here needs char const!"<<endl;
                    value='a';//容错处理
                }
                //登录符号表
                if(search_symtable(name,nowfunc_index,ptrOFsymtable)){
                    error(REDECLARE);
                }
                else{
                    enter_symtable(name,K_CONST,T_CHAR,value);
                }
            }
            else{
                error(NEEDIDENTIFIER);//cout<<"here needs identifier!"<<endl;
                skip(0);//skip to , or ;
            }
        }while(sym_type==COMMA);
    }
    else{
        //cout<<"here needs int/char!"<<endl;
        error(NEEDINTORCHAR);
        skip(1);//skip to ;
    }
    //cout<<line_count<<" is const defination!"<<endl;
    if(sym_type!=SEMICOLON) error(NEEDSEMICOLON);//cout<<"need ; here!"<<endl;
    else insymbol();

}

void var_def(){
    string name="";//记录当前变量的name
    int value=0;//记录当前变量的value
    int type=0;//记录当前变量的值类型
    if(sym_type==INTSYM) type=T_INT;//先记录下是什么type，以便之后填表
    else type=T_CHAR;
    do{
        insymbol();
        if(sym_type!=IDENTIFIER){
            //cout<<"here needs identifier!"<<endl;
            error(NEEDIDENTIFIER);
            skip(0);//skip to , or ;
            continue;
        }
        name=sym_id;
        insymbol();
        if(sym_type==LBRACKET){
            insymbol();
            /*if(sym_type==PLUSYM||sym_type==MINUSYM){
                //cout<<"need unsigned int!"<<endl;
                error(NEEDUNSIGNEDINT);
                insymbol();
            }
            if(sym_type!=INTCON){
                //cout<<"need integer here!"<<endl;
                error(NEEDUNSIGNEDINTINTCON);
                skip(0);//skip to , or ;
                continue;
            }*/
            if(sym_type!=INTCON){
                error(NEEDUNSIGNEDINT);
                skip(0);//skip to , or ;
                continue;
            }
            else if(sym_type==INTCON&&sym_num==0){
                error(NEEDUNSIGNEDINT);
                skip(0);//skip to , or ;
                continue;
            }
            value=sym_num;//value赋值为数组大小
            insymbol();
            if(sym_type!=RBRACKET){
                //cout<<"need ] here!"<<endl;
                error(NEEDRBRACKET);
            }
            else insymbol();
            //登录符号表
            if(search_symtable(name,nowfunc_index,ptrOFsymtable)){
                error(REDECLARE);
            }
            else {
                enter_symtable(name,K_ARRAY,type,value);
                //生成中间代码
                emit("ary",str2mid[type],int2string(value),name);
            }
        }
        else{
            if(search_symtable(name,nowfunc_index,ptrOFsymtable)){
                error(REDECLARE);
            }
            else{
                enter_symtable(name,K_VAR,type,0);
                //生成中间代码
                emit("var",str2mid[type]," ",name);
            }
        }
    }while(sym_type==COMMA);
    //cout<<line_count<<" is variable defination!"<<endl;
    if(sym_type!=SEMICOLON) error(NEEDSEMICOLON);//cout<<"need ; here!"<<endl;
    else insymbol();

}
void factor(){
    int loc=0;//记录标识符在符号表的位置
    int signflag=1;
    string name=" ";
    string var1=" ",var2=" ",var3=" ";
    if(sym_type==IDENTIFIER){
        name=sym_id;//记下来标识符的名字
        loc=search_symtable(sym_id,1,ptrOFsymtable);
        /*insymbol();
        if(sym_type==LPARENT){
            call_sent();
        }*/
        if(loc==0){//增加因子标识符未定义的报错
            error(UNDECLAREIDENTIFIER);
            last_type=T_INT;
            last_var="1";
            insymbol();
        }
        else if(sym_table[loc].kind==K_FUNCTION){
            call_sent();
            if(sym_table[loc].type==T_VOID){
                error(ERRORVOIDCALL);
                last_type=T_INT;//容错处理
                last_var="1";//容错处理
            }
            else{
                last_type=sym_table[loc].type;
                var3=nextempvar();
                emit("gtr"," "," ",var3);
                last_var=var3;
            }
        }
        else if(sym_table[loc].kind==K_ARRAY){
            insymbol();
            if(sym_type!=LBRACKET){
                error(NEEDLBRACKET);
            }
            else if(sym_type==LBRACKET){
                insymbol();
                expression();
                if(last_type!=T_INT){
                    error(ERRORARRAYNUM);//如果不是整型报错，容错处理
                }
                else if(last_var[0]>='0'&&last_var[0]<='9'){
                    if(str2int(last_var)>=sym_table[loc].value){
                        error(OUTOFINDEX);
                    }
                }
                var3=nextempvar();
                var2=last_var;
                var1=name;
                emit("gta",var1,var2,var3);
                last_var=var3;
                last_type=sym_table[loc].type;
                if(sym_type!=RBRACKET){
                    //cout<<"here needs ]"<<endl;
                    error(NEEDRBRACKET);
                }
                else insymbol();
            }
        }
        else{//之后补充常量、变量、参数
            if(sym_table[loc].kind==K_CONST){
                last_var=int2string(sym_table[loc].value);
                last_type=sym_table[loc].type;
            }
            else{
                last_var=sym_table[loc].name;
                last_type=sym_table[loc].type;
                var3=nextempvar();
                emit("asg",last_var," ",var3);
                last_var=var3;
            }
            insymbol();
        }
    }
    else if(sym_type==LPARENT){
        insymbol();
        expression();
        if(sym_type!=RPARENT){
            //cout<<"here needs )"<<endl;
            error(NEEDRPARENT);
        }
        else insymbol();
        //在讨论区中老师说过如果加上括号以后，相当于强制转换为int型
        last_type=T_INT;
    }
    else if(sym_type==PLUSYM||sym_type==MINUSYM){
        if(sym_type==MINUSYM) signflag=-1;
        insymbol();
        sym_num=sym_num*signflag;
        last_var=int2string(sym_num);
        last_type=T_INT;
        insymbol();
    }
    else if(sym_type==INTCON){
        last_var=int2string(sym_num);
        last_type=T_INT;
        insymbol();
    }
    else if(sym_type==CHARCON){
        last_var=int2string((int)sym_char);
        last_type=T_CHAR;
        insymbol();
    }
    else{
        //cout<<"wrong factor!"<<endl;
        error(ERRORFACTOR);
        //之后容错处理
        last_var="1";
        last_type=T_INT;
    }
    //cout<<line_count<<" is factor!"<<endl;
}
void term(){
    string var1=" ",var2=" ",var3=" ";
    int optype=0;
    int type=0;//存储返回值类型
    factor();
    type=last_type;
    var1=last_var;
    while(sym_type==TIMESYM||sym_type==DIVSYM){
        type=T_INT;//如果进行了运算，则一定是int型
        if(sym_type==TIMESYM) optype=1;
        else optype=-1;
        insymbol();
        factor();
        var3=nextempvar();
        var2=last_var;
        if(optype==1) emit("mul",var1,var2,var3);
        else emit("div",var1,var2,var3);
        last_var=var3;
        var1=last_var;
    }
    last_type=type;
    //cout<<line_count<<" is term!"<<endl;
}
void expression(){
    string var1=" ",var2=" ",var3=" ";
    int optype=0;//存储计算类型
    int type=0;//存储表达式结果类型
    if(sym_type==PLUSYM||sym_type==MINUSYM){
        if(sym_type==MINUSYM) optype=-1;
        else optype=1;
        insymbol();
    }
    term();
    type=last_type;
    if(optype) type=T_INT;//只要有了正负号，表达式的值强制变为int
    if(optype==-1){
        var1=last_var;
        var3=nextempvar();
        emit("mul",var1,"-1",var3);
        last_var=var3;
    }
    var1=last_var;
    while(sym_type==PLUSYM||sym_type==MINUSYM){
        type=T_INT;//只要参加了运算，表达式的值强制转换为int
        if(sym_type==PLUSYM) optype=1;
        else optype=-1;
        insymbol();
        term();
        var3=nextempvar();
        var2=last_var;
        if(optype==1) emit("add",var1,var2,var3);
        else emit("mis",var1,var2,var3);
        last_var=var3;
        var1=last_var;
    }
    last_type=type;
    //cout<<line_count<<" is expression!"<<endl;
}
void condition(string _label){//都是反着的，如>就生成<=
    string var1=" ",var2=" ";
    int op=0;
    expression();
    if(last_type!=T_INT){
        error(ERRORCONDITION);
    }
    var1=last_var;
    if(sym_type>=LESSYM&&sym_type<=NEQLSYM){
        op=sym_type;//记下运算符
        insymbol();
        expression();
        if(last_type!=T_INT){
            error(ERRORCONDITION);
        }
        var2=last_var;
        if(op==LESSYM) emit("bge",var1,var2,_label);
        else if(op==LEQSYM) emit("bgt",var1,var2,_label);
        else if(op==GTRSYM) emit("ble",var1,var2,_label);
        else if(op==GEQSYM) emit("blt",var1,var2,_label);
        else if(op==EQLSYM) emit("bne",var1,var2,_label);
        else emit("beq",var1,var2,_label);
    }
    else{
        emit("beq",var1,"0",_label);
    }
    //cout<<line_count<<" is condition!"<<endl;
}
void if_sent(){
    emit("#IF_BEGIN"," "," "," ");
    string endlabel="IF"+nextlabel()+"END";
    insymbol();
    if(sym_type!=LPARENT){
        //cout<<"here needs ("<<endl;
        error(NEEDLPARENT);
    }
    else insymbol();
    condition(endlabel);
    if(sym_type!=RPARENT){
        //cout<<"here needs )"<<endl;
        error(NEEDRPARENT);
    }
    else insymbol();
    emit("#IF_STATE"," "," "," ");
    if(sentence()==0){
        //cout<<"wrong sentence"<<endl;
        //skip();
        error(NEEDSENTENCE);
        return;
    }
    emit("set"," "," ",endlabel);
    //cout<<line_count<<" is if sentence!"<<endl;
}
void while_sent(){
    emit("#WHILE_BEGIN"," "," "," ");
    string label=nextlabel();
    string beginlabel="WHILE"+label+"BEGIN";
    string endlabel="WHILE"+label+"END";
    emit("set"," "," ",beginlabel);
    insymbol();
    if(sym_type!=LPARENT){
        //cout<<"here needs ("<<endl;
        error(NEEDLPARENT);
    }
    else insymbol();
    condition(endlabel);
    if(sym_type!=RPARENT){
        //cout<<"here needs )"<<endl;
        error(NEEDRPARENT);
    }
    else insymbol();
    if(sentence()==0){
        //cout<<"wrong sentence"<<endl;
        //skip();
        error(NEEDSENTENCE);
        return;
    }
    emit("jmp"," "," ",beginlabel);
    emit("set"," "," ",endlabel);
    //cout<<line_count<<" is while sentence!"<<endl;
}
void switch_sent(){
    emit("#SWITCH_BEGIN"," "," "," ");
    int type1=0;//记录switch的类型
    //int type2=0;//记录case的类型
    int signflag=1;//用于计算整数的正负
    string var1=" ",var2=" ";
    string label=" ";//用于case中跳转的标签
    string endlabel="SWITCH"+nextlabel()+"END";
    int case_count=0;
    insymbol();
    if(sym_type!=LPARENT){
        //cout<<"here need ("<<endl;
        error(NEEDLPARENT);
    }
    else insymbol();
    expression();
    type1=last_type;
    var1=last_var;
    if(sym_type!=RPARENT){
        //cout<<"here need }"<<endl;
        error(NEEDRPARENT);
    }
    else insymbol();
    if(sym_type!=LBRACE){
        //cout<<"here need {"<<endl;
        error(NEEDLBRACE);
    }
    else insymbol();
    while(sym_type==CASESYM){
        insymbol();
        /*if(sym_type!=INTCON&&sym_type!=CHARCON){
            cout<<"here needs const!"<<endl;//到时候根据符号表进行判断
        }*/
        //////判断是否是常量，处理记录生成比较语句/////////////
        if(sym_type==PLUSYM||sym_type==MINUSYM){
            if(sym_type==MINUSYM) signflag=-1;
            insymbol();
            if(sym_type!=INTCON){
                error(NEEDINTCON);
                sym_num=1;
            }
            sym_num=sym_num*signflag;
            var2=int2string(sym_num);
            if(type1!=T_INT) error(UNMATCHEDSWITCH);
            label="CASE"+nextlabel()+"END";
            emit("bne",var1,var2,label);
            insymbol();
        }
        else if(sym_type==INTCON){
            var2=int2string(sym_num);
            if(type1!=T_INT) error(UNMATCHEDSWITCH);
            label="CASE"+nextlabel()+"END";
            emit("bne",var1,var2,label);
            insymbol();
        }
        else if(sym_type==CHARCON){
            var2=int2string((int)sym_char);
            if(type1!=T_CHAR) error(UNMATCHEDSWITCH);
            label="CASE"+nextlabel()+"END";
            emit("bne",var1,var2,label);
            insymbol();
        }
        else {
            error(NEEDCONST);
            skip(5);//skip to :
        }
        //////////开始判断冒号///////////////
        if(sym_type!=COLON){
            //cout<<"here needs :"<<endl;
            error(NEEDCOLON);
        }
        else insymbol();
        if(sentence()==0){
            error(NEEDSENTENCE);
        }
        emit("jmp"," "," ",endlabel);
        emit("set"," "," ",label);
        case_count++;
    }
    if(case_count==0){
        error(NEEDCASE);
    }
    if(sym_type==DEFAULTSYM){
        insymbol();
        if(sym_type!=COLON){
            //cout<<"here needs :"<<endl;
            error(NEEDCOLON);
        }
        else insymbol();
        if(sentence()==0){
            error(NEEDSENTENCE);
        }
    }
    if(sym_type!=RBRACE){
        //cout<<"here needs }"<<endl;
        error(NEEDRBRACE);
    }
    else insymbol();
    emit("set"," "," ",endlabel);
    //cout<<line_count<<" is switch-case sentence!"<<endl;
}
void scanf_sent(){
    int loc=0;//记录标识符在符号表中的位置
    insymbol();
    if(sym_type!=LPARENT){
        //cout<<"here needs ("<<endl;
        error(NEEDLPARENT);
    }
    else insymbol();
    if(sym_type!=IDENTIFIER){
        //cout<<"here needs identifier!"<<endl;
        error(NEEDIDENTIFIER);
        skip(3);//skip to , or )
    }
    else{
        loc=search_symtable(sym_id,1,ptrOFsymtable);
        if(loc==0) error(UNDECLAREIDENTIFIER);
        else if(sym_table[loc].kind!=K_VAR&&sym_table[loc].kind!=K_PARAMENT){
            error(ERRORSCANF);
        }
        else{
            if(sym_table[loc].type==T_INT){
                emit("scf","int"," ",sym_table[loc].name);
            }
            else{
                emit("scf","char"," ",sym_table[loc].name);
            }
        }
        insymbol();
    }
    while(sym_type==COMMA){
        insymbol();
        if(sym_type!=IDENTIFIER){
            //cout<<"here needs identifier!"<<endl;
            error(NEEDIDENTIFIER);
            skip(3);// skip to , or )
            continue;
        }
        loc=search_symtable(sym_id,1,ptrOFsymtable);
        if(loc==0) error(UNDECLAREIDENTIFIER);
        else if(sym_table[loc].kind!=K_VAR&&sym_table[loc].kind!=K_PARAMENT){
            error(ERRORSCANF);
        }
        else{
            if(sym_table[loc].type==T_INT){
                emit("scf","int"," ",sym_table[loc].name);
            }
            else{
                emit("scf","char"," ",sym_table[loc].name);
            }
        }
        insymbol();
    }
    if(sym_type!=RPARENT){
        //cout<<"here needs )"<<endl;
        error(NEEDRPARENT);
    }
    else insymbol();
    //cout<<line_count<<" is scanf sentence!"<<endl;
}
void printf_sent(){
    string _str=" ";
    insymbol();
    if(sym_type!=LPARENT){
        //cout<<"here needs ("<<endl;
        error(NEEDLPARENT);
    }
    else insymbol();
    if(sym_type==STRCON){
        //emit("prt","str"," ",int2string(enter_strtable(sym_str)));
        _str=int2string(enter_strtable(sym_str));//先记录下要输出的字符串编号
        insymbol();
        if(sym_type==COMMA){
            insymbol();
            expression();
            if(last_type==T_CHAR){
                emit("prt","char",_str,last_var);
            }
            else{
                emit("prt","int",_str,last_var);
            }
        }
        else emit("prt"," ",_str," ");
    }
    else{
        expression();
        if(last_type==T_CHAR){
            emit("prt","char"," ",last_var);
        }
        else{
            emit("prt","int"," ",last_var);
        }
    }
    emit("prt","char"," ","10");//每个printf后输出一个换行
    if(sym_type!=RPARENT){
        //cout<<"here needs )"<<endl;
        error(NEEDRPARENT);
    }
    else insymbol();

    //cout<<line_count<<" is printf sentence!"<<endl;
}
void return_sent(){
    ifreturn=1;
    insymbol();
    if(sym_type==LPARENT){
        insymbol();
        expression();
        //如果返回值类型和当前声明类型不匹配，报错
        if(last_type!=now_functype) error(ERRORRETURNTYPE);
        if(sym_type!=RPARENT){
            //cout<<"here needs )"<<endl;
            error(NEEDRPARENT);
        }
        else insymbol();
        emit("ret"," "," ",last_var);
    }
    else{
        if(now_functype!=T_VOID) error(ERRORRETURNTYPE);
        emit("ret"," "," "," ");
    }
    //cout<<line_count<<" is return sentence!"<<endl;
}
void call_sent(){
    int loc=search_symtable(sym_id,1,ptrOFsymtable);
    int par=0;//记录当前函数的参数个数
    insymbol();
    if(sym_type!=LPARENT){
        //cout<<"here needs ("<<endl;
        error(NEEDLPARENT);
    }
    else insymbol();
    if(sym_type==RPARENT){
        if(par!=sym_table[loc].value){
            error(UNMATCHEDPARNUM);
        }
        insymbol();
        emit("cal"," "," ",sym_table[loc].name);
        //cout<<line_count<<" is call function!"<<endl;
        return;
    }
    expression();
    emit("psh"," "," ",last_var);
    par++;
    if(last_type!=sym_table[loc+par].type) error(UNMATCHEDPARTYPE);
    while(sym_type==COMMA){
        insymbol();
        expression();
        emit("psh"," "," ",last_var);
        par++;
        if(last_type!=sym_table[loc+par].type) error(UNMATCHEDPARTYPE);
    }
    if(par!=sym_table[loc].value){
        error(UNMATCHEDPARNUM);
    }
    if(sym_type!=RPARENT){
        //cout<<"here needs )"<<endl;
        error(NEEDRPARENT);
    }
    else insymbol();
    emit("cal"," "," ",sym_table[loc].name);
    //cout<<line_count<<" is call function!"<<endl;
}
void asg_sent(){
    int loc=0;//记录标识符的位置
    string var1=" ",var2=" ",var3=" ";
    var3=sym_id;//第三个变量为标识符/数组名
    loc=search_symtable(sym_id,1,ptrOFsymtable);
    int kind=sym_table[loc].kind;//记录标识符种类
    int type=sym_table[loc].type;//记录标识符值的类型
    insymbol();
    if(kind==K_ARRAY){
        if(sym_type!=LBRACKET){
            error(NEEDLBRACKET);
        }
        else insymbol();
        expression();
        //判断表达式的值是不是整型
        if(last_type!=T_INT){
            error(ERRORARRAYNUM);//必须是整型,之后就容错处理吧
        }
        else if(last_var[0]>='0'&&last_var[0]<='9'){
            if(str2int(last_var)>=sym_table[loc].value){
                error(OUTOFINDEX);
            }
        }
        var2=last_var;
        if(sym_type!=RBRACKET){
            error(NEEDRBRACKET);
        }
        else insymbol();

    }
    /*if(sym_type==LBRACKET){
        insymbol();
        expression();
        if(sym_type!=RBRACKET){
            //cout<<"here needs ]"<<endl;
            error(NEEDRBRACKET);
        }
        else insymbol();
    }*/
    if(sym_type!=ASGSYM){
        //cout<<"here needs = !"<<endl;
        error(NEEDEQUAL);
    }
    else insymbol();
    expression();
    //判断赋值符号两边值的类型是否一致，不一致则报错，容错处理
    if(last_type!=type){
        error(UNMATCHEDTYPE);
    }
    var1=last_var;
    if(kind==K_ARRAY){
        emit("sta",var1,var2,var3);
    }
    else{
        emit("asg",var1,var2,var3);
    }
    //cout<<line_count<<" is assign sentence!"<<endl;
}
void sent_list(){//语句列
    while(sentence());
    //cout<<line_count<<" is sentence list!"<<endl;
}

int sentence(){
    if(sym_type==IFSYM){
        if_sent();
    }
    else if(sym_type==WHILESYM){
        while_sent();
    }
    else if(sym_type==SWITCHSYM){
        switch_sent();
    }
    else if(sym_type==LBRACE){
        insymbol();
        sent_list();
        if(sym_type!=RBRACE) error(NEEDRBRACE);//cout<<"here needs }"<<endl;
        else insymbol();
    }
    else if(sym_type==SCANFSYM){
        scanf_sent();
        if(sym_type!=SEMICOLON){
            //cout<<"here needs ;"<<endl;
            error(NEEDSEMICOLON);
        }
        else insymbol();
    }
    else if(sym_type==PRINTFSYM){
        printf_sent();
        if(sym_type!=SEMICOLON){
            //cout<<"here needs ;"<<endl;
            error(NEEDSEMICOLON);
        }
        else insymbol();
    }
    else if(sym_type==RETURNSYM){
        return_sent();
        if(sym_type!=SEMICOLON){
            //cout<<"here needs ;"<<endl;
            error(NEEDSEMICOLON);
        }
        else insymbol();
    }
    else if(sym_type==SEMICOLON){
        insymbol();
    }
    else if(sym_type==IDENTIFIER){
        int loc=0;//记录该标识符在符号表的地址
            //查表，判断是变量还是函数，这里由于没有符号表，就再继续读了
            //if(var){asg_sent};
            //else if(func){call_sent);
        ////开始查表，判断是变量、数组、还是函数///////////////////
        loc=search_symtable(sym_id,1,ptrOFsymtable);
        if(loc==0){//如果未查到标识符，则报错并跳转到}或;
            error(UNDECLAREIDENTIFIER);
            skip(4);//skip to ; or }
            insymbol();
            return 1;
        }
        /*insymbol();
        if(sym_type==LPARENT){
            call_sent();
            if(sym_type!=SEMICOLON){
                //cout<<"here needs ;"<<endl;
                error(NEEDSEMICOLON);
            }
            else insymbol();
        }
        else{
            asg_sent();
            if(sym_type!=SEMICOLON){
                //cout<<"here needs ;"<<endl;
                error(NEEDSEMICOLON);
            }
            else insymbol();
        }*/
        if(sym_table[loc].kind==K_FUNCTION){
            call_sent();
        }
        else if(sym_table[loc].kind==K_CONST){
            error(ERRORCONST);
            skip(4);//skip to ; or }
            insymbol();
            return 1;
        }
        else{
            asg_sent();
        }
        if(sym_type!=SEMICOLON){
            error(NEEDSEMICOLON);
        }
        else insymbol();
    }
    else{
        return 0;//用于语句列中判断多个语句
    }
    return 1;
}

void statement(){//复合语句
    while(sym_type==CONSTSYM){//常量说明
        insymbol();
        con_def();
    }
    while(sym_type==INTSYM||sym_type==CHARSYM){
        var_def();
    }
    sent_list();
    //cout<<line_count<<" is statement!"<<endl;
}
void par_list(){//参数列表
    string name=" ";
    int type=0;
    if(sym_type==RPARENT){
        //cout<<line_count<<" is parament list!"<<endl;
        return;
    }
    if(sym_type!=INTSYM&&sym_type!=CHARSYM){
        //cout<<sym_type<<endl;
        //cout<<"here need int/char"<<endl;
        error(NEEDINTORCHAR);
        skip(3);//skip to , or )
    }
    else{
        type=sym_type-1;
        insymbol();
        if(sym_type!=IDENTIFIER){
            //cout<<"here need identifier!"<<endl;
            error(NEEDIDENTIFIER);
            skip(3);//skip to , or )
        }
        else{
            name=sym_id;
            insymbol();
        }
        /////////////查表，登录///////////////////
        if(search_symtable(name,nowfunc_index,ptrOFsymtable)){
            error(REDECLARE);
            skip(3);//skip to , or )
        }
        else{
            par_num++;
            enter_symtable(name,K_PARAMENT,type,0);
            emit("par",str2mid[type]," ",name);
        }
    }
    while(sym_type==COMMA){
        insymbol();
        if(sym_type!=INTSYM&&sym_type!=CHARSYM){
            //cout<<"here need int/char"<<endl;
            error(NEEDINTORCHAR);
            skip(3);//to , or )
            continue;
        }
        type=sym_type-1;
        insymbol();
        if(sym_type!=IDENTIFIER){
            //cout<<"here need identifier!"<<endl;
            error(NEEDIDENTIFIER);
            skip(3);
            continue;
        }
        name=sym_id;
        insymbol();
        /////////////查表，登录///////////////////
        if(search_symtable(name,nowfunc_index,ptrOFsymtable)){
            error(REDECLARE);
            skip(3);//skip to , or )
            continue;
        }
        par_num++;
        enter_symtable(name,K_PARAMENT,type,0);
        emit("par",str2mid[type]," ",name);
    }
    //cout<<line_count<<" is parament list!"<<endl;
}
void main_func(){
    insymbol();
    //////////////登录符号表,生成中间代码////////////////////////////////
    enter_symtable("main",K_FUNCTION,T_VOID,0);
    emit("fun",str2mid[now_functype]," ","main");
    //////////////////////////////////////////////////////////////////////
    if(sym_type!=LPARENT) error(NEEDLPARENT);//cout<<"here needs ( !"<<endl;
    else insymbol();
    if(sym_type!=RPARENT) error(NEEDRPARENT);//cout<<"here needs ) !"<<endl;
    else insymbol();
    if(sym_type!=LBRACE) error(NEEDLBRACE);//cout<<"here needs { !"<<endl;
    else insymbol();
    statement();
    if(sym_type!=RBRACE) error(NEEDRBRACE);//cout<<"here needs } !"<<endl;
    ////////填函数表。释放符号栈，生成中间代码///////////////
    enter_functable("main",nowfunc_index,0,var_num);//这句之后再考虑
    popsym_table();
    emit("end"," "," ","main");

    //注意，这里不再insymbol!改用nextchar测试
    while(ctemp!=-1){
        if(ctemp==' '||ctemp=='\t'||ctemp=='\n'){
            nextchar();
            continue;
        }
        cout<<"Wrong symbol after main function!"<<endl;
        break;
    }
    //cout<<line_count<<" is main function!"<<endl;
}
void func_def(){//函数定义
    ////////////////////////////
    //int type=0;//存储当前定义函数的返回值类型
    string name="";
    while(sym_type==INTSYM||sym_type==CHARSYM||sym_type==VOIDSYM){
        ifreturn=0;//清空返回计数器
        //首先定指针：
        nowfunc_index=ptrOFsymtable;
        ////////////////////////////////////////////////
        now_functype=sym_type-1;//与symbol的枚举正好差1，减去即可
        insymbol();
        if(now_functype==T_VOID&&sym_type==MAINSYM){
            main_func();
            ifmain=1;
            return;
        }
        if(sym_type!=IDENTIFIER){
            //cout<<"needs identifier here!"<<endl;
            error(NEEDIDENTIFIER);
            skip(2);//skip to int/char/void
            continue;
        }
        name=sym_id;
        //查表、登录、生成中间代码
        if(search_symtable(name,1,ptrOFsymtable)){
            error(REDECLARE);
            skip(2);//skip to int/char/void
            continue;
        }
        enter_symtable(name,K_FUNCTION,now_functype,0);//填表
        emit("fun",str2mid[now_functype]," ",name);
        //////////////继续///////////////
        insymbol();
        if(sym_type!=LPARENT){
            //cout<<"needs ( here!"<<endl;
            error(NEEDLPARENT);
        }
        else insymbol();
        par_num=0;//参数计数器置零
        par_list();
        sym_table[nowfunc_index].value=par_num;//回填符号表中函数的参数
        if(sym_type!=RPARENT){
            //cout<<"needs ) here!"<<endl;
            error(NEEDRPARENT);
        }
        insymbol();
        if(sym_type!=LBRACE){
            //cout<<"needs { here!"<<endl;
            error(NEEDLBRACE);
        }
        insymbol();
        statement();
        //////////如果没有返回语句就报错//////////////
        /*if(now_functype!=T_VOID&&ifreturn==0){
            error(NORETURN);
        }*/
        if(ifreturn==0){
            if(now_functype!=T_VOID) error(NORETURN);
            //else emit("ret"," "," "," ");
        }
        if(sym_type!=RBRACE){
            //cout<<"needs } here!"<<endl;
            error(NEEDRBRACE);
        }
        /////////回填函数表，符号表、释放符号表栈/////////////////
        enter_functable(name,nowfunc_index,par_num,var_num);//之后再考虑是否保留
        //sym_table[nowfunc_index].value=par_num;//回填符号表中函数的参数=========>这句话必须移动到前面，否则函数的自我调用就不适用了
        popsym_table();
        emit("end"," "," ",name);
        //cout<<line_count<<" is function defination!"<<endl;
        insymbol();
    }
    //cout<<"need int/char/void"<<endl;
    error(ERRORDECLARE);
}

void program(){//在启动之前必须预读一个insymbol
    while(sym_type==CONSTSYM){//常量说明
        insymbol();
        con_def();
    }

    while(sym_type==INTSYM||sym_type==CHARSYM){//变量说明
        type_reg=sym_type;//记录下类型以及源文件指针，便于之后的回溯
        inptr_reg=ptrOFincode;
        ctemp_reg=ctemp;
        insymbol();

        /*if(sym_type!=IDENTIFIER){
            cout<<"here needs identifier!"<<endl;
            //skip();
            continue;
        }*///这句删掉，因为在对应部分还会判断
        insymbol();
        if(sym_type==COMMA||sym_type==LBRACKET||sym_type==SEMICOLON){
            sym_type=type_reg;
            ptrOFincode=inptr_reg;
            ctemp=ctemp_reg;
            var_def();
            continue;
        }
        else if(sym_type==LPARENT){
            sym_type=type_reg;
            ptrOFincode=inptr_reg;
            ctemp=ctemp_reg;
            break;
        }
        else{
            //cout<<"Wrong char!"<<endl;
            error(ERRORDECLARE);
            skip(2);//skip to int or char or void
            continue;
        }
    }

    func_def();//进入函数处理

    if(ifmain==0){
        error(NOMAINFUNC);
    }

    //cout<<line_count<<" is program!"<<endl;
}

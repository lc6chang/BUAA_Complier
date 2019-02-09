#include<iostream>
#include<string>
#include<fstream>
#include"Global.h"
using namespace std;
////////////////////
ofstream outmips;//输出mips流
////////////////////
int ifbetter=0;//是否进行优化
int nowmain=0;//现在是否是主函数
string nowname=" ";//现在函数的名字
////////////////////
Addrele Addr_table[MAX_OF_ADDRTABLE];//变量地址表
int ptrOFaddrtable=1;//变量地址表的指针
int nowmidcode=0;//记录当前翻译到第几条中间代码了
int totalvar=0;//记录当前部分的变量个数
int totalpar=0;//记录当前部分的参数个数
int pushnum=0;//记录当前已经push的参数个数
int str2int(string _str){
    unsigned i;
    int result=0;
    for(i=0;i<_str.length();i++){
        result=result*10;
        result+=_str[i]-'0';
    }
    return result;
}
int search_addr(string _name){
    int i;
    for(i=ptrOFaddrtable-1;i>0;i--){
        if(Addr_table[i].name==_name) return i;
    }
    return 0;
}
void enter_addr(string _name,int _type,int _address,int _ifglobal){
    int i=ptrOFaddrtable;
    Addr_table[i].name=_name;
    Addr_table[i].type=_type;
    Addr_table[i].address=_address;
    Addr_table[i].ifglobal=_ifglobal;
    ptrOFaddrtable++;
}
string getloc(string _name,int offset,string reg){
    int loc=search_addr(_name);
    if(Addr_table[loc].ifglobal){
        if(Addr_table[loc].type==ARY){
            if(reg==" ") return int2string((Addr_table[loc].address+offset)*4)+"($gp)";
            else{
                outmips<<"sll "+reg+","+reg+",2"<<endl;
                outmips<<"add "+reg+","+reg+",$gp"<<endl;
                return int2string(Addr_table[loc].address*4)+"("+reg+")";
            }
        }
        return int2string(Addr_table[loc].address*4)+"($gp)";
    }
    else if(Addr_table[loc].type==PAR) return int2string(Addr_table[loc].address*4)+"($fp)";
    else {
        if(Addr_table[loc].type==ARY){
            if(reg==" ") return int2string((Addr_table[loc].address+offset)*4)+"($sp)";
            else{
                outmips<<"sll "+reg+","+reg+",2"<<endl;
                outmips<<"add "+reg+","+reg+",$sp"<<endl;
                return int2string(Addr_table[loc].address*4)+"("+reg+")";
            }
        }
        return int2string(Addr_table[loc].address*4)+"($sp)";
    }
}
void retasm(){
    string _var3=midcode_table[nowmidcode].var3;
    if(_var3!=" "){
        if((_var3[0]>='0'&&_var3[0]<='9')||_var3[0]=='-'){
            outmips<<"ori $v0,$0,"+_var3<<endl;
        }
        else{
            _var3=getloc(_var3,0," ");
            outmips<<"lw $v0,"+_var3<<endl;
        }
    }
    outmips<<"j "+nowname+"_end"<<endl;
}
void calasm(){
    string _var3=midcode_table[nowmidcode].var3;
    //首先将push后的栈指针下移//
    outmips<<"addi $sp,$sp,"+int2string(pushnum*(-4))<<endl;
    //pushnum=0;//千万记得pushnum要归零！！！！！！！！！！
    outmips<<"jal "+midcode_table[nowmidcode].var3<<endl;
    //下面需要恢复栈指针
    outmips<<"addi $sp,$sp,"+int2string(pushnum*4)<<endl;
    //下面需要将pushnum做减法，减去call函数的参数个数
    pushnum=pushnum-func_table[search_functable(_var3)].parcount;
}
void pshasm(){
    string _var3=midcode_table[nowmidcode].var3;
    pushnum++;
    if((_var3[0]>='0'&&_var3[0]<='9')||_var3[0]=='-'){
        outmips<<"ori $t0,$0,"+_var3<<endl;
        outmips<<"sw $t0,"+int2string(pushnum*(-4))+"($sp)"<<endl;
    }
    else{
        outmips<<"lw $t0,"+getloc(_var3,0," ")<<endl;
        outmips<<"sw $t0,"+int2string(pushnum*(-4))+"($sp)"<<endl;
    }
}
void gtrasm(){
    string _var3=midcode_table[nowmidcode].var3;
    outmips<<"sw $v0,"+getloc(_var3,0," ")<<endl;
}
void gtaasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var2=midcode_table[nowmidcode].var2;
    string _var3=midcode_table[nowmidcode].var3;
    if(_var2[0]>='0'&&_var2[0]<='9'){
        outmips<<"lw $t0,"+getloc(_var1,str2int(_var2)," ")<<endl;
        outmips<<"sw $t0,"+getloc(_var3,0," ")<<endl;
    }
    else{
        outmips<<"lw $t1,"+getloc(_var2,0," ")<<endl;
        outmips<<"lw $t0,"+getloc(_var1,0,"$t1")<<endl;
        outmips<<"sw $t0,"+getloc(_var3,0," ")<<endl;
    }
}
void staasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var2=midcode_table[nowmidcode].var2;
    string _var3=midcode_table[nowmidcode].var3;
    if(_var2[0]>='0'&&_var2[0]<='9'){
        if((_var1[0]>='0'&&_var1[0]<='9')||_var1[0]=='-') outmips<<"ori $t0,$0,"+_var1<<endl;
        else outmips<<"lw $t0,"+getloc(_var1,0," ")<<endl;
        outmips<<"sw $t0,"+getloc(_var3,str2int(_var2)," ")<<endl;
    }
    else{
        if((_var1[0]>='0'&&_var1[0]<='9')||_var1[0]=='-') outmips<<"ori $t0,$0,"+_var1<<endl;
        else outmips<<"lw $t0,"+getloc(_var1,0," ")<<endl;
        outmips<<"lw $t1,"+getloc(_var2,0," ")<<endl;
        outmips<<"sw $t0,"+getloc(_var3,0,"$t1")<<endl;
    }
}
void asgasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var3=midcode_table[nowmidcode].var3;
    if((_var1[0]>='0'&&_var1[0]<='9')||_var1[0]=='-') outmips<<"ori $t0,$0,"+_var1<<endl;
    else outmips<<"lw $t0,"+getloc(_var1,0," ")<<endl;
    outmips<<"sw $t0,"+getloc(_var3,0," ")<<endl;
}
void scfasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var3=midcode_table[nowmidcode].var3;
    if(_var1=="int") outmips<<"ori $v0,$0,5"<<endl;
    else if(_var1=="char") outmips<<"ori $v0,$0,12"<<endl;
    outmips<<"syscall"<<endl;
    outmips<<"sw $v0,"+getloc(_var3,0," ")<<endl;
}
void prtasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var2=midcode_table[nowmidcode].var2;
    string _var3=midcode_table[nowmidcode].var3;
    if(_var2!=" "){
        outmips<<"la $a0,str_"+_var2<<endl;
        outmips<<"ori $v0,$0,4"<<endl;
        outmips<<"syscall"<<endl;
    }
    if(_var1=="int"){
        if((_var3[0]>='0'&&_var3[0]<='9')||_var3[0]=='-') outmips<<"ori $a0,$0,"+_var3<<endl;
        else outmips<<"lw $a0,"+getloc(_var3,0," ")<<endl;
        outmips<<"ori $v0,$0,1"<<endl;
        outmips<<"syscall"<<endl;
    }
    else if(_var1=="char"){
        if(_var3[0]>='0'&&_var3[0]<='9') outmips<<"ori $a0,$0,"+_var3<<endl;
        else outmips<<"lw $a0,"+getloc(_var3,0," ")<<endl;
        outmips<<"ori $v0,$0,11"<<endl;
        outmips<<"syscall"<<endl;
    }
    /*else if(_var1=="str"){
        outmips<<"la $a0,str_"+_var3<<endl;
        outmips<<"ori $v0,$0,4"<<endl;
    }
    outmips<<"syscall"<<endl;*/
}
void addasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var2=midcode_table[nowmidcode].var2;
    string _var3=midcode_table[nowmidcode].var3;
    if((_var1[0]>='0'&&_var1[0]<='9')||_var1[0]=='-') outmips<<"ori $t0,$0,"+_var1<<endl;
    else outmips<<"lw $t0,"+getloc(_var1,0," ")<<endl;
    if((_var2[0]>='0'&&_var2[0]<='9')||_var2[0]=='-') outmips<<"ori $t1,$0,"+_var2<<endl;
    else outmips<<"lw $t1,"+getloc(_var2,0," ")<<endl;
    outmips<<"add $t0,$t0,$t1"<<endl;
    outmips<<"sw $t0,"+getloc(_var3,0," ")<<endl;
}
void misasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var2=midcode_table[nowmidcode].var2;
    string _var3=midcode_table[nowmidcode].var3;
    if((_var1[0]>='0'&&_var1[0]<='9')||_var1[0]=='-') outmips<<"ori $t0,$0,"+_var1<<endl;
    else outmips<<"lw $t0,"+getloc(_var1,0," ")<<endl;
    if((_var2[0]>='0'&&_var2[0]<='9')||_var2[0]=='-'){
        outmips<<"ori $t1,$0,"+_var2<<endl;
        outmips<<"mul $t1,$t1,-1"<<endl;
    }
    else{
        outmips<<"lw $t1,"+getloc(_var2,0," ")<<endl;
        outmips<<"mul $t1,$t1,-1"<<endl;
    }
    outmips<<"add $t0,$t0,$t1"<<endl;
    outmips<<"sw $t0,"+getloc(_var3,0," ")<<endl;
}
void mulasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var2=midcode_table[nowmidcode].var2;
    string _var3=midcode_table[nowmidcode].var3;
    if((_var1[0]>='0'&&_var1[0]<='9')||_var1[0]=='-') outmips<<"ori $t0,$0,"+_var1<<endl;
    else outmips<<"lw $t0,"+getloc(_var1,0," ")<<endl;
    if((_var2[0]>='0'&&_var2[0]<='9')||_var2[0]=='-') outmips<<"ori $t1,$0,"+_var2<<endl;
    else outmips<<"lw $t1,"+getloc(_var2,0," ")<<endl;
    outmips<<"mul $t0,$t0,$t1"<<endl;
    outmips<<"sw $t0,"+getloc(_var3,0," ")<<endl;
}

void divasm(){
    string _var1=midcode_table[nowmidcode].var1;
    string _var2=midcode_table[nowmidcode].var2;
    string _var3=midcode_table[nowmidcode].var3;
    if((_var1[0]>='0'&&_var1[0]<='9')||_var1[0]=='-') outmips<<"ori $t0,$0,"+_var1<<endl;
    else outmips<<"lw $t0,"+getloc(_var1,0," ")<<endl;
    if((_var2[0]>='0'&&_var2[0]<='9')||_var2[0]=='-') outmips<<"ori $t1,$0,"+_var2<<endl;
    else outmips<<"lw $t1,"+getloc(_var2,0," ")<<endl;
    outmips<<"div $t0,$t0,$t1"<<endl;
    outmips<<"sw $t0,"+getloc(_var3,0," ")<<endl;
}
void beqasm(){
    string _op=midcode_table[nowmidcode].op;
    string _var1=midcode_table[nowmidcode].var1;
    string _var2=midcode_table[nowmidcode].var2;
    string _var3=midcode_table[nowmidcode].var3;
    if((_var1[0]>='0'&&_var1[0]<='9')||_var1[0]=='-') outmips<<"ori $t0,$0,"+_var1<<endl;
    else outmips<<"lw $t0,"+getloc(_var1,0," ")<<endl;
    if((_var2[0]>='0'&&_var2[0]<='9')||_var2[0]=='-') outmips<<"ori $t1,$0,"+_var2<<endl;
    else outmips<<"lw $t1,"+getloc(_var2,0," ")<<endl;
    outmips<<_op+" $t0,$t1,"+_var3<<endl;
}
void funasm(){
    /////////////如果是主函数，则不需要进行各种保存，节省指令条数,否则保护现场//////////////
    if(nowmain){
        outmips<<"ori $fp,$sp,0"<<endl;//设置帧指针，这句话之后也可以省略，目前为了整体性先加上
        outmips<<"addi $sp,$sp,"+int2string(totalvar*(-4))<<endl;//为临时变量分配空间
    }
    else{
        outmips<<"addi $sp,$sp,-4"<<endl;//栈指针下移
        outmips<<"sw $fp,0($sp)"<<endl;//保存帧指针
        outmips<<"addi $fp,$sp,4"<<endl;//重新设置帧指针
        outmips<<"addi $sp,$sp,-4"<<endl;//栈指针下移
        outmips<<"sw $fp,0($sp)"<<endl;//保存栈指针
        outmips<<"addi $sp,$sp,-4"<<endl;//栈指针下移
        outmips<<"sw $ra,0($sp)"<<endl;//保存返回地址
        outmips<<"addi $sp,$sp,"+int2string(totalvar*(-4))<<endl;//分配变量的存储空间
    }
    ///////////////////////开始各种中间代码的翻译//////////////////////////////
    while(midcode_table[nowmidcode].op!="end"){
        if(midcode_table[nowmidcode].op=="ret") retasm();
        else if(midcode_table[nowmidcode].op=="set") outmips<<midcode_table[nowmidcode].var3+":"<<endl;
        else if(midcode_table[nowmidcode].op=="jmp") outmips<<"j "+midcode_table[nowmidcode].var3<<endl;
        else if(midcode_table[nowmidcode].op=="cal") calasm();
        else if(midcode_table[nowmidcode].op=="psh") pshasm();
        else if(midcode_table[nowmidcode].op=="gtr") gtrasm();
        else if(midcode_table[nowmidcode].op=="gta") gtaasm();
        else if(midcode_table[nowmidcode].op=="sta") staasm();
        else if(midcode_table[nowmidcode].op=="asg") asgasm();
        else if(midcode_table[nowmidcode].op=="scf") scfasm();
        else if(midcode_table[nowmidcode].op=="prt") prtasm();
        else if(midcode_table[nowmidcode].op=="add") addasm();
        else if(midcode_table[nowmidcode].op=="mis") misasm();
        else if(midcode_table[nowmidcode].op=="mul") mulasm();
        else if(midcode_table[nowmidcode].op=="div") divasm();
        else if(midcode_table[nowmidcode].op=="beq"||
                midcode_table[nowmidcode].op=="bne"||
                midcode_table[nowmidcode].op=="bge"||
                midcode_table[nowmidcode].op=="bgt"||
                midcode_table[nowmidcode].op=="ble"||
                midcode_table[nowmidcode].op=="blt") beqasm();//包含了一族跳转指令的处理
        nowmidcode++;
    }
    /////////////////////开始恢复///////////////////////////////////////////////
    if(nowmain){
        outmips<<"main_end:"<<endl;
    }
    else{
        outmips<<nowname+"_end:"<<endl;
        outmips<<"lw $ra,-12($fp)"<<endl;//恢复返回地址
        outmips<<"lw $sp,-8($fp)"<<endl;//恢复栈指针
        outmips<<"lw $fp,-4($fp)"<<endl;//恢复帧指针
        //outmips<<"addi $sp,$sp,"+int2string(4*totalpar)<<endl;//将push到栈顶的参数清空
        outmips<<"jr $ra"<<endl;//跳转回调用函数
    }
}
void tomips(string _file){

    int i;
    outmips.open(_file.data());
    /////首先写入字符串表/////
    outmips<<".data"<<endl;
    for(i=1;i<ptrOFstrtable;i++){
        outmips<<"str_"+int2string(i)+": .asciiz \""+str_table[i]+"\""<<endl;
    }
    outmips<<".text"<<endl;
    /////之后写入全局变量/////////////////////////////
    while(midcode_table[nowmidcode].op=="var"||midcode_table[nowmidcode].op=="ary"){
        if(midcode_table[nowmidcode].op=="var"){
            enter_addr(midcode_table[nowmidcode].var3,VAR,totalvar,1);
            totalvar++;
        }
        else{
            enter_addr(midcode_table[nowmidcode].var3,ARY,totalvar,1);
            totalvar+=str2int(midcode_table[nowmidcode].var2);
        }
        nowmidcode++;
    }
    //outmips<<"ori $gp,$sp,0"<<endl;//记下全局变量运行栈的栈指针
    outmips<<"addi $sp,$sp,"+int2string(totalvar*(-4))<<endl;//为全局变量开辟空间
    outmips<<"ori $gp,$sp,0"<<endl;//记下全局变量运行栈的栈指针
    outmips<<"j main"<<endl;//跳转到main函数
    ///////////////开始处理函数////////////////////////////////////////////////
    while(nowmidcode!=ptrOFmidcode&&midcode_table[nowmidcode].op=="fun"){
        nowname=midcode_table[nowmidcode].var3;
        if(nowname=="main") nowmain=1;
        outmips<<midcode_table[nowmidcode].var3+":"<<endl;
        //cout<<midcode_table[nowmidcode].var3+":"<<endl;
        totalvar=0;//将变量计数器清零
        int temp_ptrOFaddr=ptrOFaddrtable;//记录下当前符号表的指针，以便之后回退
        totalpar=func_table[search_functable(midcode_table[nowmidcode].var3)].parcount;//获取当前函数的参数个数
        int temp_totalpar=totalpar;//存储当前参数个数，用作参数填表
        nowmidcode++;

        while(midcode_table[nowmidcode].op=="par"){//将参数填表
            temp_totalpar--;
            enter_addr(midcode_table[nowmidcode].var3,PAR,temp_totalpar,0);
            nowmidcode++;
        }

        while(midcode_table[nowmidcode].op=="var"||midcode_table[nowmidcode].op=="ary"){//将声明的变量填表
            if(midcode_table[nowmidcode].op=="var"){
                enter_addr(midcode_table[nowmidcode].var3,VAR,totalvar,0);
                totalvar++;
            }
            else{
                enter_addr(midcode_table[nowmidcode].var3,ARY,totalvar,0);
                totalvar+=str2int(midcode_table[nowmidcode].var2);
            }
            nowmidcode++;
        }

        int temp_nowmidcode=nowmidcode;
        while(midcode_table[nowmidcode].op!="end"){//将临时变量填表
            if(midcode_table[nowmidcode].var1[0]=='#'){
                if(search_addr(midcode_table[nowmidcode].var1)==0){
                    enter_addr(midcode_table[nowmidcode].var1,VAR,totalvar,0);
                    totalvar++;
                }
            }
            if(midcode_table[nowmidcode].var2[0]=='#'){
                if(search_addr(midcode_table[nowmidcode].var2)==0){
                    enter_addr(midcode_table[nowmidcode].var2,VAR,totalvar,0);
                    totalvar++;
                }
            }
            if(midcode_table[nowmidcode].var3[0]=='#'){
                if(search_addr(midcode_table[nowmidcode].var3)==0){
                    enter_addr(midcode_table[nowmidcode].var3,VAR,totalvar,0);
                    totalvar++;
                }
            }
            nowmidcode++;
        }
        nowmidcode=temp_nowmidcode;//恢复中间代码指针
        funasm();//开始递归函数体的代码生成
        ////////////恢复各种计数指针///////////////////
        ptrOFaddrtable=temp_ptrOFaddr;
        nowmidcode++;//跳过end语句
    }
    outmips.close();
}


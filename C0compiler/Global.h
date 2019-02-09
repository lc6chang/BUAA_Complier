#ifndef GLOBAL_H
#define GLOBAL_H

//宏定义
#define MAX_OF_MIDCODE 3000
#define MAX_OF_SYMTABLE 1000
#define MAX_OF_FUNCTABLE 100
#define MAX_OF_STRTABLE 100
#define MAX_OF_ADDRTABLE 1000

#include <string>

using namespace std;

//源文件及输出文件
//FileRW.cpp
extern string incode;
extern unsigned int ptrOFincode;

void getINcode(string file);


//词法分析
//GetSym.cpp
enum Symbol_type{
    CONSTSYM,INTSYM,CHARSYM,VOIDSYM,MAINSYM,
    IFSYM,WHILESYM,SWITCHSYM,CASESYM,
    DEFAULTSYM,SCANFSYM,PRINTFSYM,RETURNSYM,
    PLUSYM,MINUSYM,TIMESYM,DIVSYM,
    LESSYM,LEQSYM,GTRSYM,GEQSYM,ASGSYM,EQLSYM,NEQLSYM,
    COMMA,COLON,SEMICOLON,
    LPARENT,RPARENT,LBRACKET,RBRACKET,LBRACE,RBRACE,
    INTCON,CHARCON,STRCON,IDENTIFIER
};
extern int sym_count;//单词计数器
extern int line_count;//行号计数器
extern int column_count;//列号计数器
extern int sym_type;//读到单词的类型
extern int sym_num;//读到数字的值
extern char sym_char;//读到字符的值
extern string sym_str;//读到字符串的值
extern string sym_id;//读到的标识符
extern char ctemp;//缓存的char

void nextchar();
void insymbol();
void outsym();

//语法分析
//Grammar.cpp
void program();
int sentence();
void expression();
void call_sent();
string int2string(int n);

//错误处理
//Error.cpp
extern int err_num;
enum Error_type{
    LEADINGZERO,NEEDQUOTA,NEEDOUBLEQUOTA,NEEDEQUAL,ERRORCHAR,ERRORSYMBOL,
    NEEDINTCON,NEEDIDENTIFIER,NEEDCHARCON,NEEDINTORCHAR,ERRORDECLARE,
    NEEDUNSIGNEDINT,NEEDRBRACKET,NEEDSEMICOLON,NEEDLPARENT,NEEDRPARENT,
    NEEDLBRACE,NEEDRBRACE,NOMAINFUNC,EXTERNSYMBOL,NEEDCOLON,NEEDSENTENCE,
    ERRORFACTOR,NEEDCASE,REDECLARE,TABLEOVERFLOW,UNDECLAREIDENTIFIER,
    ERRORCONST,ERRORARRAYNUM,UNMATCHEDTYPE,ERRORVOIDCALL,UNMATCHEDPARNUM,
    UNMATCHEDPARTYPE,NORETURN,ERRORRETURNTYPE,ERRORSCANF,ERRORCONDITION,
    NEEDCONST,UNMATCHEDSWITCH,OUTOFINDEX,NEEDLBRACKET
};
void error(int n);
void skip(int n);

//符号表管理
//Table.cpp
enum Table_kind{
    K_CONST,K_VAR,K_ARRAY,K_FUNCTION,K_PARAMENT
};
enum Table_type{
    T_INT,T_CHAR,T_VOID
};

typedef struct{
    string name;//符号名
    int kind;//符号类型,常量、变量、数组、函数或参数
    int type;//符号值的类型，int、char、void
    int value;//符号的值(常量的值、数组的大小、(函数的参数个数)
//    int varnum;//函数变量&临时变量的个数
}Symbol_ele;
////////函数表，之后可能会继续修改////////////////////////
typedef struct{
    string name;//符号名
    int addr;//在符号表中的下标
    int parcount;//参数个数
    int varcount;//变量个数
}Func_ele;

extern Symbol_ele sym_table[MAX_OF_SYMTABLE];//符号表及指针
extern int ptrOFsymtable;
extern Func_ele func_table[MAX_OF_FUNCTABLE];
extern int ptrOFfunctable;
extern string str_table[MAX_OF_STRTABLE];//字符串表
extern int ptrOFstrtable;
extern int nowfunc_index;//当前函数在符号表sym_table地址

int search_symtable(string _name,int _begin,int _end);
int search_functable(string _name);
void enter_symtable(string _name, int _kind, int _type, int _value);
void enter_functable(string _name, int _addr, int _parcount, int _varcount);
int enter_strtable(string _str);
void popsym_table();
void out_table();

//生成中间代码
//Midcode.cpp
typedef struct{
    string op;
    string var1;
    string var2;
    string var3;
}Midcode_ele;

extern Midcode_ele midcode_table[MAX_OF_MIDCODE];
extern int ptrOFmidcode;

void emit(string _op,string _var1,string _var2,string _var3);
string nextempvar();
string nextlabel();
void out_midcode();
void outMidcode(string file);

//生成目标代码
//Tomips.cpp
enum MipsType{
    PAR,VAR,ARY
};
typedef struct{
    string name;
    int type;
    int address;
    int ifglobal;
}Addrele;

extern Addrele Addr_table[MAX_OF_ADDRTABLE];
extern int ptrOFaddrtable;
extern int ifbetter;

int str2int(string _str);
void tomips(string _file);

//优化中间代码
//Better.cpp




#endif // GLOBAL_H

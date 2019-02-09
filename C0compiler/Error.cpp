#include<iostream>
#include"Global.h"
using namespace std;

int err_num=0;

void printerror(){
    cout<<"ERROR "<<err_num<<": At line: "<<line_count<<", Near symbol ";
    outsym();
}
void skip(int n){
    switch(n){
        case 0:{//, or ;
            while(sym_type!=COMMA&&sym_type!=SEMICOLON){
                insymbol();
            }
            break;
        }
        case 1:{// ;
            while(sym_type!=SEMICOLON){
                insymbol();
            }
            break;
        }
        case 2:{// int or char or void
            while(sym_type!=INTSYM&&sym_type!=CHARSYM&&sym_type!=VOIDSYM){
                insymbol();
            }
            break;
        }
        case 3:{ // , or )
            while(sym_type!=COMMA&&sym_type!=RPARENT){
                insymbol();
            }
            break;
        }
        case 4:{ // ; or }
            while(sym_type!=SEMICOLON&&sym_type!=RBRACE){
                insymbol();
            }
            break;
        }
        case 5:{// :
            while(sym_type!=COLON){
                insymbol();
            }
            break;
        }
    }
}
void error(int n){
    err_num++;
    switch(n){
        case LEADINGZERO:{
            printerror();
            cout<<", Error leading zero"<<endl;
            break;
        }
        case NEEDQUOTA:{
            printerror();
            cout<<", Needs ' "<<endl;
            break;
        }
        case NEEDOUBLEQUOTA:{
            printerror();
            cout<<", Needs \" "<<endl;
            break;
        }
        case NEEDEQUAL:{
            printerror();
            cout<<", Needs = "<<endl;
            break;
        }
        case ERRORCHAR:{
            printerror();
            cout<<", This is error char const"<<endl;
            break;
        }
        case ERRORSYMBOL:{
            printerror();
            cout<<", This is error symbol"<<endl;
            break;
        }
        case NEEDINTCON:{
            printerror();
            cout<<", Needs int const"<<endl;
            break;
        }
        case NEEDIDENTIFIER:{
            printerror();
            cout<<", Needs identifier"<<endl;
            break;
        }
        case NEEDCHARCON:{
            printerror();
            cout<<", Needs char const"<<endl;
            break;
        }
        case NEEDINTORCHAR:{
            printerror();
            cout<<", Needs int/char"<<endl;
            break;
        }
        case ERRORDECLARE:{
            printerror();
            cout<<", This is error declare"<<endl;
            break;
        }
        case NEEDUNSIGNEDINT:{
            printerror();
            cout<<", Needs usigned int const which not equals 0"<<endl;
            break;
        }
        case NEEDRBRACKET:{
            printerror();
            cout<<", Needs ] "<<endl;
            break;
        }
        case NEEDSEMICOLON:{
            printerror();
            cout<<", Needs ; "<<endl;
            break;
        }
        case NEEDLPARENT:{
            printerror();
            cout<<", Needs ( "<<endl;
            break;
        }
        case NEEDRPARENT:{
            printerror();
            cout<<", Needs ) "<<endl;
        }
        case NEEDLBRACE:{
            printerror();
            cout<<", Needs { "<<endl;
            break;
        }
        case NEEDRBRACE:{
            printerror();
            cout<<", Needs } "<<endl;
            break;
        }
        case NOMAINFUNC:{
            printerror();
            cout<<", No main function"<<endl;
            break;
        }
        case EXTERNSYMBOL:{
            printerror();
            cout<<", Extern symbol after main function"<<endl;
            break;
        }
        case NEEDCOLON:{
            printerror();
            cout<<", Needs : "<<endl;
            break;
        }
        case NEEDSENTENCE:{
            printerror();
            cout<<", Needs sentence"<<endl;
            break;
        }
        case ERRORFACTOR:{
            printerror();
            cout<<", This is error factor"<<endl;
            break;
        }
        case NEEDCASE:{
            printerror();
            cout<<", Need at least 1 case"<<endl;
            break;
        }
        case REDECLARE:{
            printerror();
            cout<<", The identifier is redeclared"<<endl;
            break;
        }
        case TABLEOVERFLOW:{
            printerror();
            cout<<", Table over flow"<<endl;
            break;
        }
        case UNDECLAREIDENTIFIER:{
            printerror();
            cout<<", The identifier is not declared"<<endl;
            break;
        }
        case ERRORCONST:{
            printerror();
            cout<<", The const can't be used here"<<endl;
            break;
        }
        case ERRORARRAYNUM:{
            printerror();
            cout<<", The num in array[num] must be int type"<<endl;
            break;
        }
        case UNMATCHEDTYPE:{
            printerror();
            cout<<", The two value on both sides of the operator is unmatched"<<endl;
            break;
        }
        case ERRORVOIDCALL:{
            printerror();
            cout<<", The void function can't be called here"<<endl;
            break;
        }
        case UNMATCHEDPARNUM:{
            printerror();
            cout<<", The number of parament is unmatched of function"<<endl;
            break;
        }
        case UNMATCHEDPARTYPE:{
            printerror();
            cout<<", The type of parament is unmatched of the function declare"<<endl;
            break;
        }
        case NORETURN:{
            printerror();
            cout<<", No return sentence in function"<<endl;
            break;
        }
        case ERRORRETURNTYPE:{
            printerror();
            cout<<", The return value type is unmatched of function declare"<<endl;
            break;
        }
        case ERRORSCANF:{
            printerror();
            cout<<", This kind of identifier can't be used in scanf"<<endl;
            break;
        }
        case ERRORCONDITION:{
            printerror();
            cout<<", The type of the expression in condition must be int"<<endl;
            break;
        }
        case NEEDCONST:{
            printerror();
            cout<<", Here needs const"<<endl;
            break;
        }
        case UNMATCHEDSWITCH:{
            printerror();
            cout<<", The case type is unmatched of switch type"<<endl;
            break;
        }
        case OUTOFINDEX:{
            printerror();
            cout<<", The array is out of index"<<endl;
            break;
        }
        case NEEDLBRACKET:{
            printerror();
            cout<<", Needs [ "<<endl;
            break;
        }
    }
}

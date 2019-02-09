#include <iostream>
#include <string>
#include "Global.h"

using namespace std;

int main()
{
    string _incodename;
    cout<<"请输入源代码路径: "<<endl;
    getline(cin,_incodename);
    getINcode(_incodename);

    string _midcode;
    cout<<"请输入保存的中间代码路径："<<endl;
    getline(cin,_midcode);

    string _mipscode;
    cout<<"请输入目标Mips代码的路径："<<endl;
    getline(cin,_mipscode);

    /*cout<<"是否进行优化？输入1为优化，0为不优化："<<endl;
    cin>>ifbetter;*/

    nextchar();//预读一个char
    insymbol();
    program();



    if(err_num==0){
        cout<<"Compile successfully!"<<endl;
        if(ifbetter){
            //进行优化
        }
        outMidcode(_midcode);
        tomips(_mipscode);
    }
    else{
        cout<<"Compile error, not finished! Total: "<<err_num<<" errors!"<<endl;
    }
    /*cout<<endl<<"Here's midcode:"<<endl;
    out_midcode();*/
    //cout<<endl<<endl<<"Here's symbol table!"<<endl;
    //out_table();
    //
    //cout<<nextempvar()<<endl;
    //cout<<nextempvar()<<endl;
    return 0;
}

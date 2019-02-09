#include<iostream>
#include<cstdlib>
#include<fstream>
#include<cassert>
#include "Global.h"

string incode="";
unsigned int ptrOFincode=0;

void getINcode(string file){
    fstream infile;

    infile.open(file.data());
    if(!infile){
        cout<<"No such file!"<<endl;
        cout<<"Compile error, not finished!"<<endl;
        exit(0);
    }

    char c=' ';
    infile.get(c);
    while(true){
        incode=incode+c;
        infile.get(c);
        if(infile.eof()){
            break;
        }
    }
    infile.close();
}




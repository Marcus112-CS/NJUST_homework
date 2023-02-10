//
// Created by DELL on 2022/4/26.
//
#include <bits/stdc++.h>
#include "Lexer_Head.h"
#include "Phraser_Head.h"
#include "Syn_Head.h"
extern int error_number;
using namespace std;
int main() {
    cout<<"     Compiler Of 李广通    version 1.0     "<<endl<<endl;
    cout<<"      The functions are listed below "<<endl<<endl;
    cout<<"1.词法分析 ( NFA->DFA 子集法 )"<<endl;
    cout<<"2.语法分析 ( LR(1) 文法 )"<<endl;
    cout<<endl<<"---------请输入指令来运行上述两个分析器---------"<<endl<<endl;
    cout<<"Available instruction : "<<endl;
    cout<<"1. run task1"<<endl<<"2. run task2"<<endl<<"3. run both"<<endl<<"4. exit"<<endl<<endl;
    cout<<"-------please input your instruction-------"<<endl;
    cout<<">>";
    string instruction;
    int flag=0;
    while(1){
        flag=0;
        getline(cin,instruction);
        if(instruction=="run task1"){
            Lexer_Analyze();
            if(error_number)
                cout<<endl<<"你可以根据提示信息修改错误后再运行task 2,也可以直接带错运行";
            flag=1;
        }
        if(instruction=="run task2"){
            Phraser_Analyze();
        }
        if(instruction=="run both"){
            Lexer_Analyze();
            Phraser_Analyze();
        }
        if(instruction=="exit"){
            break;
        }
        if(flag==1)
            cout<<endl<<endl;
        else
            cout<<endl;
        cout<<"-------please input your instruction-------"<<endl;
        cout<<">>";
    }
    cout<<endl<<"-----------------exit--------------------"<<endl;
    return 0;
}


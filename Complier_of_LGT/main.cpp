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
    cout<<"     Compiler Of ���ͨ    version 1.0     "<<endl<<endl;
    cout<<"      The functions are listed below "<<endl<<endl;
    cout<<"1.�ʷ����� ( NFA->DFA �Ӽ��� )"<<endl;
    cout<<"2.�﷨���� ( LR(1) �ķ� )"<<endl;
    cout<<endl<<"---------������ָ����������������������---------"<<endl<<endl;
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
                cout<<endl<<"����Ը�����ʾ��Ϣ�޸Ĵ����������task 2,Ҳ����ֱ�Ӵ�������";
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


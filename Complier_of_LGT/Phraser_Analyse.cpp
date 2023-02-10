//
// Created by DELL on 2022/4/26.
//
#include <bits/stdc++.h>
#include "Phraser_Head.h"
extern int error_number;
using namespace std;

extern set<char> VT_2;
extern set<char> VN_2;
extern set<char> VN_FOR_TITLE;
extern vector<char> chars_for_analyse;
extern unordered_map<char,bool> VN_ifcanbe_epsilon;
extern map<int,pair<char,string>>  PRODUCTION_FOR_ACTION;
extern map<char,RIGHT_SET> PRODUCTION_SOURCE;
extern map<char,set<char>> VN_FIRST;
extern map<char,bool> VN_getfirst_ifdone;
extern map<char,set<int>> line_search_for_closure;
extern set<pair<pair<int,char>,int>> MOVE_OF_NEW_STATE;
extern set<pair<pair<int,char>,int>,compare> GOTO_TABLE;
extern set<pair<pair<int,char>,ACTION_item>,compare> ACTION_TABLE;
extern map<pair<int ,char>,int > goto_for_search;
extern map<pair<int ,char>,int > action_for_search;
extern unordered_map<int,PROJECT_SET> NEW_STATE;
int STATE_NUM_MINUS_ONE;

void Phraser_Analyze(){
    cout<<endl<<endl<<"      task 2 is running ..."<<endl<<endl;
    if(error_number){
        cout<<"源程序中还有 "<<error_number<<" 项内容不合法,如想运行 ”task 2 —— Phraser Analyse” "<<endl<<"请参考上方错误信息进行修改再重新打开 "<<endl;
    }
    // 语法分析的文法可以先行进行 处理  后期加入else
    init();
    vn_epsilon_process();   // if vn epsilon
    vn_get_FIRST_SET();    // first
    LR_1_CREATE();          // 项目集合规范组构建（状态转换关系）
    get_ACTION_and_GOTO();
    cout<<endl;
    LR_1_ANALYSIS();
}
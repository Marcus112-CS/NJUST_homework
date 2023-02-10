//
// Created by DELL on 2022/4/26.
//
#include <bits/stdc++.h>
#include "Phraser_Head.h"
#include "Lexer_Head.h"
using namespace std;

set<char> VT_2;
set<char> VN_2;   // 不包括@
set<char> VN_FOR_TITLE; // GOTO 表头
vector<char> chars_for_analyse; // 用于存储要移进——规约的字符串
unordered_map<char,bool> VN_ifcanbe_epsilon;    // 各个vn是否可以推出空  GOAL 1
map<int,pair<char,string>>  PRODUCTION_FOR_ACTION;    // 用于action表进行规约时根据行号查找规约到哪个vn
map<char,RIGHT_SET> PRODUCTION_SOURCE;      // init将正规式存入的形式
map<char,set<char>> VN_FIRST;   // VN的first集
map<char,bool> VN_getfirst_ifdone;  // 各个是否已经完成求取first集的操作
map<char,set<int>> line_search_for_closure;  // 统计同一vn为左部的产生式行号
set<pair<pair<int,char>,int>,compare> MOVE_OF_NEW_STATE;  // 纪录新状态的转化matrix
set<pair<pair<int,char>,int>,compare> GOTO_TABLE;  // GOTO表
set<pair<pair<int,char>,ACTION_item>,compare> ACTION_TABLE; // ACTION表
map<pair<int ,char>,int > goto_for_search;          // 用于打印和查询扫描
map<pair<int ,char>,ACTION_item > action_for_search;
unordered_map<int,PROJECT_SET> NEW_STATE;  // 用数字做index的新状态
int STATE_NUM=0;   // 状态从0开始计数
extern map<int,pair<pair<int,string>,char>> for_task2; // 根据字符号查询行号\位置; 字符index，<行号，字符>

ofstream output_info_2;           //  输出文法分析的具体信息
ofstream output_2;                //  输出yes|no 并给出错误行号和提示信息

void split_2 (string line,vector<string>& words){
    int left=0,right=0;
    bool ifhasinword=false;  // 是否已经运行到单词中
    for (int i = 0; i < line.length(); ++i) {
        if((line[i]=='{'||line[i]=='}'||line[i]==';'||line[i]=='>'||line[i]==')')&&i==line.length()-1) {    //line[i]=='{'||line[i]=='}'||line[i]==';'||line
            string * k=new string;
            left=i;
            k->append(line,left,1);
            words.push_back(*k);
            break;
        }
        if(line[i]!=' '&&line[i]!='\t'&&ifhasinword==false){
            left=i;
            ifhasinword=true;
            continue;
        }
        if((line[i]==' '||line[i]=='\t')&&ifhasinword){
            right=i-1;
            ifhasinword= false;
            string * k=new string;
            k->append(line,left,right-left+1);
            words.push_back(*k);
            continue;
        }
        if(i==line.length()-1&&ifhasinword){
            if(line[i]!='\n')
                right=i;
            else
                right=i-1;
            string * k=new string;
            k->append(line,left,right-left+1);
            words.push_back(*k);
            continue;
        }
    }
}
void init (){
//    ifstream source("../2_in_token.txt");
    VT_2.clear();
    VN_2.clear();   // 不包括@
    VN_FOR_TITLE.clear(); // GOTO 表头
    chars_for_analyse.clear(); // 用于存储要移进——规约的字符串
    VN_ifcanbe_epsilon.clear();    // 各个vn是否可以推出空  GOAL 1
    PRODUCTION_FOR_ACTION.clear();    // 用于action表进行规约时根据行号查找规约到哪个vn
    PRODUCTION_SOURCE.clear();      // init将正规式存入的形式
    VN_FIRST.clear();   // VN的first集
    VN_getfirst_ifdone.clear();  // 各个是否已经完成求取first集的操作
    line_search_for_closure.clear();  // 统计同一vn为左部的产生式行号
    MOVE_OF_NEW_STATE.clear();  // 纪录新状态的转化matrix
    GOTO_TABLE.clear();  // GOTO表
    ACTION_TABLE.clear(); // ACTION表
    goto_for_search.clear();          // 用于打印和查询扫描
    action_for_search.clear();
    NEW_STATE.clear();  // 用数字做index的新状态
    STATE_NUM=0;   // 状态从0开始计数

    ifstream grammar_2("../2_in_grammar.txt");
//    ifstream grammar_2("../2_in_grammar_test.txt");
    output_2.open("../2_out.txt");
    output_info_2.open("../info_about_task2.txt");
    string line;
    int line_index=0;
    vector<string> words;
    while(getline(grammar_2,line)) {
        line_index++;
        // line_search_for_closure初始化
        if(line_search_for_closure.find(line[0])!=line_search_for_closure.end()){
            line_search_for_closure.find(line[0])->second.insert(line_index);
        }
        else{
            set<int > temp;
            temp.insert(line_index);
            line_search_for_closure.insert({line[0],temp});
        }
        // PRODUCTION_FOR_ACTION初始化
        string rr=line.substr(3);
        PRODUCTION_FOR_ACTION.insert({line_index,{line[0],rr}});
        // VT_2、VN_2初始化
        VN_2.insert(line[0]);
        for (int i = 3; i < line.length(); ++i) {
            if(line[i]<'A'||line[i]>'Z')
                VT_2.insert(line[i]);
        }
        // PRODUCTION_SOURCE
        auto pointer=PRODUCTION_SOURCE.find(line[0]);
        if(pointer!=PRODUCTION_SOURCE.end()){  // ling[0]为左部的已经存在
            rs temp(line);
            pointer->second.insert(temp);
        }
        else{       // 第一个ling[0]为左部的产生式子
            rs temp(line);
            RIGHT_SET newrs;
            newrs.insert(temp);
            PRODUCTION_SOURCE.insert({line[0],newrs});
        }
        words.clear();
//        split_2(line,words);
//        for(auto i:words)
//            cout<<i<<" ";
//        cout<<endl;
    }
    for(auto i:VN_2){             // 首先将其置全false
        VN_getfirst_ifdone.insert({i,false});
    }
    output_info_2<<"                                   语法分析文法 相关信息统计(已排序处理)  "<<endl;
    output_info_2<<"产生式条数 : "<<line_index<<endl;
    output_info_2<<"Vn数目 : "<<VN_2.size()<<endl;
    output_info_2<<"        { ";
    for(auto i:VN_2)
        output_info_2<<i;
    output_info_2<<" }"<<endl;
    output_info_2<<"Vt数目 : "<<VT_2.size()<<endl;
    output_info_2<<"        { ";
    for(auto i:VT_2)
        output_info_2<<i;
    output_info_2<<" }"<<endl;
}
void GOTO_ACTION_TABLE_show (){
    output_info_2<<"                            LR(1) GOTO-ACTION 表"<<endl;
    output_info_2<<"      ";
    for(auto i:VT_2)
        output_info_2<<i<<"       ";
    for(auto i:VN_FOR_TITLE)
        output_info_2<<i<<"       ";
    output_info_2<<endl;
    for (int i = 0; i < NEW_STATE.size(); ++i) {
        output_info_2<<" "<<setiosflags(ios::left)<<setw(5)<<i;
        int counter=0;
        for(auto j:VT_2){
            counter++;
            if(counter<VT_2.size()){
                if(action_for_search.find({i,j})!=action_for_search.end())
                    output_info_2<<setiosflags(ios::left)<<setw(8)<<action_for_search.find({i,j})->second.get_output_string();
                else
                    output_info_2<<setiosflags(ios::left)<<setw(8)<<"        ";
            }
            else{
                if(action_for_search.find({i,j})!=action_for_search.end())
                    output_info_2<<setiosflags(ios::left)<<setw(4)<<action_for_search.find({i,j})->second.get_output_string();
                else
                    output_info_2<<setiosflags(ios::left)<<setw(4)<<" ";
            }
        }
        output_info_2<<"|   ";
        for(auto j:VN_FOR_TITLE){
            if(goto_for_search.find({i,j})!=goto_for_search.end())
                output_info_2<<setiosflags(ios::left)<<setw(8)<<goto_for_search.find({i,j})->second;
            else
                output_info_2<<setiosflags(ios::left)<<setw(8)<<"        ";
        }
        output_info_2<<endl;
    }
}
void get_stack_char_vector_version (stack<char> character,vector<char> &character_fornow){
    character_fornow.clear();
    stack<char> temp;
    int num=character.size();
    for (int i = 0; i < num; ++i) {
        temp.push(character.top());
        character.pop();
    }
    for (int i = 0; i < num; ++i) {
        character_fornow.push_back(temp.top());
        temp.pop();
    }
//    for(auto i:character_fornow)  // 恢复现场 stack默认传的是指针
//        character.push(i);
}
void get_stack_state_vector_version (stack<int> state,vector<int> &state_fornow){
    state_fornow.clear();
    stack<int> temp;
    int num=state.size();
    for (int i = 0; i < num; ++i) {
        temp.push(state.top());
        state.pop();
    }
    for (int i = 0; i < num; ++i) {
        state_fornow.push_back(temp.top());
        temp.pop();
    }
//    for(auto i:state_fornow)
//        state.push(i);
}
bool if_equal (PROJECT_SET a,PROJECT_SET b){
    if(a.size()==b.size()){
        bool ifhave=false;
        for(auto i:a){  // 对每个a中的item 根据line_index 在b中查找
            ifhave=false;
            for(auto j:b){   // 有一个没有找到都为空
                if(j==i){
                    ifhave=true;
                    break;
                }
            }
            if(!ifhave)
                return false;
        }
        return true;
    }
    else
        return false;
}
bool if_is_new_ACTION_item (pair<pair<int,char>,ACTION_item> temp){
    bool res=false;
    for(auto i:ACTION_TABLE){
        if(temp.first==i.first&&temp.second==i.second)
            return true;
    }
    return false;
}
int if_in_new_state (PROJECT_SET aft_set){
    for(auto i:NEW_STATE){
        if(if_equal(i.second,aft_set)){
           return i.first;
        }
    }
    return -1;
}
project_set_ITEM init_project_set_item (int line_index){
    project_set_ITEM new_item;
    auto point=PRODUCTION_FOR_ACTION.find(line_index);
    new_item.line_index=line_index;
    new_item.tail='#';
    new_item.left_Vn=point->second.first;
    new_item.right.push_back('.');
    for (int i = 0; i < point->second.second.length(); ++i) {
        new_item.right.push_back(point->second.second[i]);
    }
    return new_item;
}
PROJECT_SET GOTO_SET (PROJECT_SET pre_set_copy,char v){
    PROJECT_SET res;
    for(auto i:pre_set_copy){
        for (int j = 0; j < i.right.size(); ++j) {
            if(i.right[j]=='.'&&j+1<i.right.size()){
                if(i.right[j+1]==v){         // 向后move一位
                   i.right.erase(i.right.begin()+j);
                   i.right.insert(i.right.begin()+j+1,'.');
                   res.insert(i);
                }
                break;
            }
        }
    }
    return res;
}
set<char> get_tail (string temp){
    vector<char> characters;
    set<char> res;
    for (int i = 0; i < temp.length(); ++i) {
        characters.push_back(temp[i]);
    }
    int counter=0;               //  统计一共合并了几个符号的first集，如果全vn可出空，则加入@
    for(auto i:characters){
        if(VN_2.count(i)&&VN_ifcanbe_epsilon.find(i)->second){  // 是vn且可以出空
           counter++;
           for(auto j:VN_FIRST.find(i)->second){
               if(j!='@')
                   res.insert(j);
           }
        }
        else{       // 不可出空的vn||vt  直接停止了不会再向下走
           if(VT_2.count(i)) {
              res.insert(i);
              break;
           }
           else{    // 不可出空的vn
               for(auto j:VN_FIRST.find(i)->second){
                   res.insert(j);
               }
           }
        }
    }
    if(counter==characters.size())
        res.insert('@');
    return res;
}
void vn_epsilon_process (){       // 要求vn必须是大写英文字母
    set<char> VN_can_epsilon,VN_not_epsilon,VN_unknow; // 先求出定能|不能求出闭包的状态;
    bool all_have_vt=true;
    bool if_added_to_must=false;       // 是否已加入前两个绝对集合
    int counter=0;
    for(auto i:PRODUCTION_SOURCE){
        counter=0;
        if_added_to_must= false;
        for(auto j:i.second){
           if(j.if_a_single_epsilon){   // if have ->@
               if_added_to_must=true;
               VN_can_epsilon.insert(i.first);
               goto here;
           }
           for(auto k:j.right_inorder){         // 如果一个vn的所有产生式的右部的都含有vn,必不能产生空
               if(j.right_for_compare[0]<'A'||j.right_for_compare[0]>'Z'){
                   counter++;
                   break;
               }     // 如果右部中存在终结符
           }
        }
        if(counter==i.second.size()){
            VN_not_epsilon.insert(i.first);
            if_added_to_must=true;
        }
        if(!if_added_to_must)
            VN_unknow.insert(i.first);
        here : ;
    }
    bool ifhave_added=false;
    while (!VN_unknow.empty()){
        set<char> VN_unknow_temp;
        VN_unknow_temp.clear();
        for(auto i:VN_unknow)
            VN_unknow_temp.insert(i);
        for(auto i:VN_unknow_temp){    // 对第一轮扫描不确定能否推出空的vn  将其全部为vn的所有右部式子进行判断
            ifhave_added=false;
            for(auto j:PRODUCTION_SOURCE.find(i)->second){
                counter=0;
                for(auto k:j.right_inorder){
                    if(j.right_for_compare[0]>='A'&&j.right_for_compare[0]<='Z'&&VN_can_epsilon.find(k)!=VN_can_epsilon.end()){
                        counter++;
                    }
                }
                if(counter==j.right_inorder.size()){  // 该右部全是vn，如果全都能出空，则出空
                    bool all_can_be_epsilon=true;
                    for(auto k:j.right_inorder){
                        if(VN_not_epsilon.find(k)!=VN_not_epsilon.end()){
                            all_can_be_epsilon= false;
                            break;
                        }
                    }
                    if(all_can_be_epsilon){
                        ifhave_added= true;
                        VN_can_epsilon.insert(i);
                        VN_unknow.erase(i);
                        goto here2;
                    }
                }
            }
            here2 : ;
            if(!ifhave_added){
                VN_not_epsilon.insert(i);
                VN_unknow.erase(i);
            }
        }
    }
    for(auto i:VN_can_epsilon)
        VN_ifcanbe_epsilon.insert({i,true});
    for(auto i:VN_not_epsilon)
        VN_ifcanbe_epsilon.insert({i,false});
    output_info_2<<"各Vn能否推出空 : "<<endl;
    output_info_2<<"          ";
    for(auto i:VN_ifcanbe_epsilon){  // 输出第一行vn
        output_info_2<<i.first<<"   ";
    }
    output_info_2<<endl;
    output_info_2<<"          ";
    for(auto i:VN_ifcanbe_epsilon){  // 输出第一行vn
        if(i.second)
            output_info_2<<setiosflags(ios::left)<<setw(4)<<"Y";
        else
            output_info_2<<setiosflags(ios::left)<<setw(4)<<"N";
    }
    output_info_2<<endl;
//    for(auto i:VN_can_epsilon)
//        cout<<i<<" ";
//    cout<<endl;
//    for(auto i:VN_not_epsilon)
//        cout<<i<<" ";
//    cout<<endl;
//    for(auto i:VN_unknow)
//        cout<<i<<" ";
//    cout<<endl;
}
void vn_get_FIRST_SET (){
    map<char,set<char>> first_temp;
    int counter=0;      // 统计 对每一个vn 的所有产生式右部 满足第一个字符为vt|@的个数
    for(auto i:VN_2){      // 对每个vn
        counter=0;
        for(auto j:PRODUCTION_SOURCE.find(i)->second){
           if(j.if_a_single_epsilon){   // 如果有->@
              if(first_temp.find(i)!=first_temp.end()){
                  first_temp.find(i)->second.insert('@');
              }
              else{
                  set<char> tt;
                  tt.insert('@');
                  first_temp.insert({i,tt});
              }
              counter++;
              continue;
           }
           if(j.right_for_compare[0]<'A'||j.right_for_compare[0]>'Z'){ // 如果右部第一个为vt
               if(first_temp.find(i)!=first_temp.end()){
                   first_temp.find(i)->second.insert(j.right_for_compare[0]);
               }
               else{
                   set<char> tt;
                   tt.insert(j.right_for_compare[0]);
                   first_temp.insert({i,tt});
               }
               counter++;
               continue;
           }
        }
        if(counter==PRODUCTION_SOURCE.find(i)->second.size()){     // 如果只根据 @和右部首字符为vt就已经是最终first集;
            VN_getfirst_ifdone.find(i)->second=true;
            VN_FIRST.insert({i,first_temp.find(i)->second}); // 加入最终的VN_FIRST
        }
    }
    set<char> VT_have_not_firsted;
    for(auto i:VN_getfirst_ifdone){   // 找到还未拿到first集的vn
        if(!i.second)
            VT_have_not_firsted.insert(i.first);
    }
    bool if_all_vnbegin_right_done=true;   // 对一个左部vn 是否所有以vn开头的右部的first集合都可以被顺利查找
    int right_epsilon_vn_num=0;      // 统计，用于判断是否一路走到头都是空


    int countert=0;
    while(!VT_have_not_firsted.empty()){
        countert++;
        if(countert==200)
            ;
        set<char> VT_have_not_firsted_temp;
        for(auto i:VT_have_not_firsted)
            VT_have_not_firsted_temp.insert(i);
        for(auto i:VT_have_not_firsted_temp){        // 对每个第一次扫描没能取到first集的vn进行循环求first
            if_all_vnbegin_right_done=true;
            for(auto j:PRODUCTION_SOURCE.find(i)->second){
                right_epsilon_vn_num=0;
                if(j.right_for_compare[0]>='A'&&j.right_for_compare[0]<='Z'){  // 如果vn开头
                    for(auto k:j.right_inorder){       // !!! 真正主体
                        if(k>='A'&&k<='Z'){  // vn
                            if(VN_getfirst_ifdone.find(k)->second){
                                if(VN_FIRST.find(k)!=VN_FIRST.end()){
                                    bool flag=true;
                                    for(auto ok:VN_FIRST.find(k)->second){
                                        if(ok!='@'){
                                            if(first_temp.find(i)!=first_temp.end())
                                                first_temp.find(i)->second.insert(ok);
                                            else{
                                                if(flag){
                                                    set<char> jjjj;
                                                    jjjj.insert(ok);
                                                    first_temp.insert({i,jjjj});
                                                }
                                                else
                                                    first_temp.find(i)->second.insert(ok);
                                            }
                                        }  // 去掉@
                                    }
                                }
                            }
                            else{    // 右部走到的当前vnfirst还没做好，直接推出等下一轮
                                if(k!=i)  // X->Xi 否则死循环
                                    if_all_vnbegin_right_done=false;
                                goto here;
                            }
                            if(VN_ifcanbe_epsilon.find(k)->second)  // 如果当前vn可以出空
                                right_epsilon_vn_num++;
                            else
                                break;
                        }
                        else{    // vn      haiyougeziranzhongzhi
                            first_temp.find(i)->second.insert(k);
                            break;
                        }
                    }
                }
                if(right_epsilon_vn_num==j.right_inorder.size())        // 右部一路为空，则first集可以加入@
                    first_temp.find(i)->second.insert('@');
            }
            here :
            if(if_all_vnbegin_right_done){  // 所有产生式都走过了，first集合已经是终结态了
                VN_getfirst_ifdone.find(i)->second=true;
                if(first_temp.find(i)!=first_temp.end())
                    VN_FIRST.insert({i,first_temp.find(i)->second});
                VT_have_not_firsted.erase(i);
            }
        }
    }
    output_info_2<<"各个Vn的First集合 : "<<endl;
    for(auto i:VN_FIRST){
        output_info_2<<"          First (";
        output_info_2<<i.first<<") = { ";
        int counter_for_print=0;
        for(auto j:i.second){
           counter_for_print++;
           if(counter_for_print==i.second.size())
               output_info_2<<j<<" }"<<endl;
           else
               output_info_2<<j<<",";
        }
    }



//    PROJECT_SET a,b;
//    if(a==b);

}
void get_CLOSURE (PROJECT_SET & bef){
    int counter=0;
    int before,after=0;  // 用于辅助一直循环求闭包直至不再扩大
    before=bef.size();
    project_set_ITEM aa,bb;

    bool flag=false;
    while(before!=after){
        if(flag)
            before=after;
        for(auto i:bef){
            counter=0;
            for(auto j=i.right.begin(); j!=i.right.end();j++){
                counter++;
                if(*j=='.'&&counter+1<=i.right.size()){  // 找到右边还有字符的点号所在的item
                    if(VN_2.count(*(j+1))) {   // 有 .vn则对.紧邻VN求闭包，同时通过计算first集合更新tail
                        string temp;
                        if(counter+2<=i.right.size())
                            temp=PRODUCTION_FOR_ACTION.find(i.line_index)->second.second.substr(counter);
                        char tail_char=i.tail;              // 更新tail前将要求闭包的子串整个统计好
                        string temp_tail(1,tail_char);
                        temp.append(temp_tail);
//                        temp=temp+temp_tail;
//                        temp=temp+i.tail;       C++ 不允许char直接转string
                        set<char> tail_set= get_tail(temp);
                        auto pointer=line_search_for_closure.find(*(j+1));  // 找到.后vn对应的所有产生式行号
                        for(auto k:pointer->second){    // k 为产生式行号
                            project_set_ITEM aa(init_project_set_item(k));
                            for(auto m:tail_set){     // 针对每个.后的vn可以出的产生式，使用不同的tail生成不同的item
                                aa.tail=m;
                                bef.insert(aa);
                            }
                        }
                    }
                    break;
                }
            }
        }
        flag=true;
        after=bef.size();
    }
}
void LR_1_CREATE (){
    // $->S 为第0条 增广文法的加入引发一系列变更
    VT_2.insert('#');  // error reason
    if(VN_ifcanbe_epsilon.find('S')->second)
        VN_ifcanbe_epsilon.insert({'$',true});
    else
        VN_ifcanbe_epsilon.insert({'$',false});
    PRODUCTION_FOR_ACTION.insert({0,{'$',"S"}});
    rs the_begin("$->S");
    RIGHT_SET first_temp;
    first_temp.insert(the_begin);
    PRODUCTION_SOURCE.insert({'$',first_temp});
    VN_FIRST.insert({'$',VN_FIRST.find('S')->second});
    // 建立第一个项目集
    PROJECT_SET pre_set;
    pre_set.insert(init_project_set_item(0));
    get_CLOSURE(pre_set);           // 取CLOSURE
    NEW_STATE.insert({STATE_NUM++,pre_set});  // 加入新状态
    stack<PROJECT_SET> s;
    s.push(pre_set);
    set<char> vt_or_vn_forconvert; // 用于GOTO的可能字符
    while(!s.empty()){
        pre_set=s.top();
        s.pop();
        int pre= if_in_new_state(pre_set),aft;  // 用于辅助LR（1）状态转化表进行插入
        vt_or_vn_forconvert.clear();
        for(auto i:pre_set){               //  查找所有.的右部终结和非终结符
            for (int j = 0; j < i.right.size(); ++j) {
                if(i.right[j]=='.'){
                    if(j+1<i.right.size()){
                        vt_or_vn_forconvert.insert(i.right[j+1]);
                        break;
                    }
                }
            }
        }
        if(vt_or_vn_forconvert.size()==0)       // 已经是终态了，不可能再向下转化
            continue;
        else{                   // 可以向下GOTO
            for(auto i:vt_or_vn_forconvert){
                // 先清空并复制样本
                PROJECT_SET pre_set_copy;  // 复制pre_set样本
                for(auto j:pre_set)
                    pre_set_copy.insert(j);
                // 进行GOTO操作并取闭包
                PROJECT_SET aft_set= GOTO_SET(pre_set_copy,i); // 转化（.移动）后初步项目集为aft_set
                get_CLOSURE(aft_set);                 //  只有闭包过程中tail才可能发生改变
                if(if_in_new_state(aft_set)==-1){
                    NEW_STATE.insert({STATE_NUM++,aft_set});  // 加入newstate
                    s.push(aft_set);
                }
                // 对LR（1）状态转化表进行插入
                aft= if_in_new_state(aft_set);
                MOVE_OF_NEW_STATE.insert({{pre,i},aft});
            }
        }
    }
    output_info_2<<"项目集规范族构建结果 : "<<NEW_STATE.size()<<" 个"<<endl;
    int counter_a=0;
    unordered_map<int,PROJECT_SET>::iterator i;
    while (1){
        if(counter_a<=NEW_STATE.size()-1)
            i=NEW_STATE.find(counter_a);
        else
            break;
        output_info_2<<"          I"<<i->first<<" :"<<endl;
        for(auto j:i->second){
            output_info_2<<"              "<<j.left_Vn<<"->";
            for(auto k:j.right){
                if(k=='.')
                    output_info_2<<"·";
                else
                    output_info_2<<k;
            }
            output_info_2<<','<<j.tail<<endl;
        }
        output_info_2<<endl;
        counter_a++;
    }
    output_info_2<<"项目集转化关系 ( MOVE_MATRIX ) : "<<endl;
    output_info_2<<"         "<<"I_before"<<"     Vt      "<<"I_after"<<endl;
    for(auto i:MOVE_OF_NEW_STATE)
        output_info_2<<"           I"<<i.first.first<<"         "<<i.first.second<<"         I"<<i.second<<endl;
    cout<<"项目集规范族构建结果 : "<<NEW_STATE.size()<<" 个"<<endl;
    counter_a=0;
    while (1){
        if(counter_a<=NEW_STATE.size()-1)
            i=NEW_STATE.find(counter_a);
        else
            break;
        cout<<"I"<<i->first<<" :"<<endl;
        int jj=0;
        for(auto j:i->second){
            cout<<"    "<<j.left_Vn<<"->";
            for(auto k:j.right){
                if(k=='.')
                    cout<<"·";
                else
                    cout<<k;
            }
            cout<<','<<j.tail;
            if(jj<i->second.size()-1)
                cout<<endl;
            jj++;
        }
        cout<<endl;
        counter_a++;
    }
    cout<<"项目集转化关系 ( MOVE_MATRIX ) : "<<endl;
    for(auto i:MOVE_OF_NEW_STATE){
        cout<<"    I"<<i.first.first<<"   "<<i.first.second<<"   "<<"I"<<i.second<<endl;
    }
//    for(auto i:pre_set){
//        cout<<i.left_Vn<<"->";
//        for(auto j:i.right)
//            cout<<j;
//        cout<<','<<i.tail;
//        cout<<endl;
//    }
}
void get_ACTION_and_GOTO (){
    // 先对goto表添加  状态转移中弧为Vn的
    for(auto i:MOVE_OF_NEW_STATE){
        if(VN_2.count(i.first.second))   // MOVE_OF_NEW_STATE中character为Vn直接添加GOTO表
            GOTO_TABLE.insert({i.first,i.second});
    }
    // 再对action表添加 状态转移中弧为Vt的
    //  S r acc添加
    for(auto i:NEW_STATE){
        for(auto j:i.second){
            if(*j.right.rbegin()=='.'){    // r和acc情况
                if(j.tail=='#'&&j.left_Vn=='$'&&*j.right.begin()=='S'){     // acc情况
                    ACTION_item tempt('a',0);
                    if(!if_is_new_ACTION_item({{i.first,'#'},tempt}))
                        ACTION_TABLE.insert({{i.first,'#'},tempt});
                    continue;
                }
                if(VT_2.count(j.tail)){            // r情况
                    ACTION_item tempt('r',j.line_index);
                    if(!if_is_new_ACTION_item({{i.first,j.tail},tempt}))
                        ACTION_TABLE.insert({{i.first,j.tail},tempt});
                }
            }
            else{     // S情况               /////////////////////////////////!!!!error
                for (int k = 0; k < j.right.size(); ++k) {
                    if(j.right[k]=='.'){
                        if(VT_2.count(j.right[k+1])){
//                            auto point =MOVE_OF_NEW_STATE.find({i.first,j.right[k+1]});
                            for(auto mm:MOVE_OF_NEW_STATE){
                                if(mm.first.first==i.first&&mm.first.second==j.right[k+1]){  // S情况
                                    ACTION_item tempt('S',mm.second);
                                    if(!if_is_new_ACTION_item({{i.first,j.right[k+1]},tempt}))
                                        ACTION_TABLE.insert({{i.first,j.right[k+1]},tempt});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // 对GOTO和ACTION的查询表进行初始化定义
    for(auto i:GOTO_TABLE){
        goto_for_search.insert({i.first,i.second});
        VN_FOR_TITLE.insert(i.first.second);
    }
    for(auto i:ACTION_TABLE)
        action_for_search.insert({i.first,i.second});
    GOTO_ACTION_TABLE_show();

    cout<<endl<<"LR(1) GOTO-ACTION 表"<<endl;
    cout<<"     ";
    for(auto i:VT_2)
        cout<<i<<"   ";
    for(auto i:VN_FOR_TITLE)
        cout<<i<<"   ";
    cout<<endl;
    for (int i = 0; i < NEW_STATE.size(); ++i) {
        cout<<setiosflags(ios::left)<<setw(3)<<i;
        int counter=0;
        for(auto j:VT_2){
            counter++;
            if(counter<VT_2.size()){
                if(action_for_search.find({i,j})!=action_for_search.end())
                    cout<<setiosflags(ios::left)<<setw(4)<<action_for_search.find({i,j})->second.get_output_string();
                else
                    cout<<setiosflags(ios::left)<<setw(4)<<"    ";
            }
            else{
                if(action_for_search.find({i,j})!=action_for_search.end())
                    cout<<setiosflags(ios::left)<<setw(4)<<action_for_search.find({i,j})->second.get_output_string();
                else
                    cout<<setiosflags(ios::left)<<setw(4)<<"    ";
            }
        }
        for(auto j:VN_FOR_TITLE){
            if(goto_for_search.find({i,j})!=goto_for_search.end())
                cout<<setiosflags(ios::left)<<setw(3)<<goto_for_search.find({i,j})->second;
            else
                cout<<setiosflags(ios::left)<<setw(3)<<"     ";
        }
        cout<<endl;
    }
}
void LR_1_ANALYSIS (){
    ifstream input_string("../2_in_string.txt");
    string temp;
    bool if_accept_success=false;   // 是否被成功接受
    getline(input_string,temp);
    for (int i = 0; i < temp.length(); ++i)     // 将待分析字符串读入
        chars_for_analyse.push_back(temp[i]);
    chars_for_analyse.push_back('#');
    cout<<"移进规约分析过程详见 info_about_task2.txt ( 实时生成 )"<<endl<<endl;
    // step 1 单独处理
    stack<int> state;          // 状态栈
    stack<char> character;      // 符号栈
    vector<int> state_fornow;   // 当前行使用方便查询和输出
    vector<char> character_fornow;
    state.push(0);
    character.push('#');
    int step_num=0;      // 步骤号
    int char_for_access=0;
    output_info_2<<"                            LR(1) 规约分析过程展示"<<endl;
    while(!if_accept_success){        // 注意引入无表项时的break
        output_info_2<<step_num++<<" ";
        if(step_num==2)
            ;
        get_stack_state_vector_version(state,state_fornow);
        get_stack_char_vector_version(character,character_fornow);
        for(auto i:character_fornow)  // 输出当前符号栈
            output_info_2<<i;
        output_info_2<<"    ";
        for(auto i:state_fornow)  // 输出当前状态栈
            output_info_2<<i<<",";
        output_info_2<<"    ";
        for(auto i:chars_for_analyse)  // 输出 输入符号串
            output_info_2<<i;
        if(action_for_search.find({state.top(),chars_for_analyse[0]})!=action_for_search.end()){  // 当前状态在action表中可以查到
            auto point=action_for_search.find({state.top(),chars_for_analyse[0]});
            if(point->second.movement==1){  // S 移进
                char_for_access++;
                state.push(point->second.number);
                character.push(chars_for_analyse[0]);
                chars_for_analyse.erase(chars_for_analyse.begin());
                output_info_2<<point->second.movement_chinese()<<endl;
            }
            if(point->second.movement==2){  // r 规约
                int line_index=point->second.number;
                auto point_production=PRODUCTION_FOR_ACTION.find(line_index);
                // state stack
                for (int i = 0; i < point_production->second.second.length(); ++i) {
                    state.pop();
                    character.pop();
                }
                state.push(goto_for_search.find({state.top(),point_production->second.first})->second);
                // character stack
                character.push(point_production->second.first);
                // chars_for_analyse  规约则待输入符号串无操作（是一个一个输入的，而只有规约可能一次规约多个）
                output_info_2<<point->second.movement_chinese()<<"( ";
                output_info_2<<point_production->second.first<<"->"<<point_production->second.second<<" )"<<endl;
            }
            if(point->second.movement==3){  // acc 接受 理想的循环出口
                if_accept_success= true;
                output_info_2<<point->second.movement_chinese()<<endl;
            }
        }
        else{  // 当前不可进规约  ！！！wait for 错误信息处理
            break;
        }
//        if_accept_success=true;  // debug
    }

//  while
    cout<<"语法判断结果 : "<<endl;
    if(if_accept_success)
        cout<<"            YES , 符合设定文法"<<endl;
    else{
        cout<<"            NO "<<endl<<"可能的错误原因如下所示 : "<<endl;
        auto point=for_task2.find(char_for_access+1); //只有移进才操作
        cout<<"错误位置为源程序 line "<<point->second.first.first<<" : "<<point->second.first.second;
        if(point->second.first.second=="return"){
            cout<<" 错误类型为"<<" main function return fault"<<endl;
        }
        else{
            if(point->second.second=='I')
                cout<<" 错误类型为"<<"KEYWORD error"<<endl;
            else if(point->second.second=='C')
                cout<<" 错误类型为"<<"IDENTIFIER define error"<<endl;
            else if(point->second.second=='F')
                cout<<" 错误类型为"<<"CONSTANT(float) define error"<<endl;
            else if(point->second.second=='O')
                cout<<" 错误类型为"<<"CONSTANT(scientific) define error";
            else if(point->second.second=='P')
                cout<<" 错误类型为"<<"CONSTANT(16) define error";
            else if(point->second.second=='E')
                cout<<" 错误类型为"<<"CONSTANT(complex) define error";
            else if(point->second.second=='K')
                cout<<" 错误类型为"<<"OPERATOR or DELIMITER error";
            else
                cout<<" 未知错误,请手动检查";
        }
//        cout<<temp[char_for_access]<<temp[char_for_access-1]<<temp[char_for_access+1];

        // 显示源程序对应行号 可以通过对token同行计数把最大数存入数组来卡界限确定错误位置
    }
}
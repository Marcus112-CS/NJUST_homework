//
// Created by DELL on 2022/4/26.
//
#include <bits/stdc++.h>
#include "Lexer_Head.h"
using namespace std;

int VT_NUM=0;  //84  终结符个数不变
int NFA_VN_NUM=0;  //33
int DFA_VN_NUM=0;
char start_state;
int DFA_start_state=0;
int pu=0;             // 动态纪录epsilon闭包和a弧转化 次数
int error_number=0;
char final_bef='S',final_new;
set<char> VT;  //NFA终结符
set<char> VN;  //NFA非终结符
set<int> DFA_VN; // DFA非终结符  无效设计，set自排序可能不同状态开头代表元素也相同
NEW_of_OLDstate O_IN_NEW;    // 真正的DFA非终结符结构设计
set<char> nfa_final_state;
map<int,pair<pair<int,string>,char>> for_task2; // 根据字符号查询行号\位置; 字符index，<行号，字符>原本类型
unordered_map<char,set<char>> VN_CLOSURE_SET;   // 终结符的闭包
unordered_map<int ,set<char>> DFA_FINAL_STATE_FOR_PRINT;  // DFA的终态map 数字号-原组成状态 由dfa_final_state 和 O_IN_NEW生成
unordered_map<int ,set<char>> DFA_NOTFINAL_STATE_FOR_PRINT;  // 同上
map<set<char>,int> dfa_state_search;   // 辅助提供通过nfa状态集合寻找dfa状态的可能 和O_IN_NEW 互反
unordered_map<char,set<char>> useable_vt_for_NFA_VN;     // 处理收集nfa中单个vn可用的vt
unordered_map<int,bool> dfa_state_ifcircleandepsilon;   // 当前状态是否已经经过了i弧转换和词法语法分析
unordered_map<int,bool> dfa_final_state;     // DFA的状态是否为终结态 （！！！！！！！！还是人为规定转换终态只要有$就算）  在O_IN_NEW中列出可以进入终态的所有状态
int dfa_vn_counter=0;          // 用于dfa状态计数插入 输出记得char int转换
int for_task2_num=1;
int line_index_for2=0;
NFA_movetable move_matrix_NFA;          // for auto 只能遍历而不能插入
NFA_movetable move_matrix_NFA_final;       // 真正的集合形式
SEARCH_NFA  move_matrix_NFA_MAP;        // 从final转化而来
DFA_movetable move_matrix_DFA;
map<pair<int ,char>,int > move_matrix_DFA_FOR_SEARCH;  // 由move_matrix_DFA演化而来，方便进行dfa寻路，词法分析
const char keyword[50][20]={"break","case","char","continue","do","default","double","bits/stdc++.h","complex","map","vector",   // auto vector std/c++.h
                            "else","float","string","using","namespace","std","for","if","int","include","long","main","return","switch","typedef","void","unsigned","while","iostream"};

FILE* source_code=NULL;         //  要分析的源程序
FILE* grammar_1=NULL;           //  词法分析用文法
ofstream output_info;           //  输出词法文法分析的具体信息
ofstream output;                //  token表输出
ofstream output_string;

bool if_inVT (char a,set<char> VT){
    if(VT.find(a)!=VT.end())
        return true;
    else
        return false;
}
bool if_inVN (char a,set<char> VN){
    if(VN.find(a)!=VN.end())
        return true;
    else
        return false;
}
int if_inNewSet (NSET & pre_set_copy,map<set<char>,int> dfa_state_search){      // 如果是已有状态，返回当前状态的数字
//    for(auto i:O_IN_NEW){
//        if(pre_set_copy.set_member==i.second)
//            return i.first;
//    }
    auto i=dfa_state_search.find(pre_set_copy.set_member);
    if(i!=dfa_state_search.end())
        return i->second;
    else
        return -1;
}
void show_NFA_movetable(NFA_movetable move_matrix_NFA_final){
    output_info<<"NFA move matrix ( NFA 状态转换表 ) "<<endl;
    output_info<<"    Vn     "<<"Vt     "<<"Vn_new     "<<endl;
    for(auto i:move_matrix_NFA_final){
        output_info<<"    "<<i.first.first<<"      "<<i.first.second<<"        ";
//        if(i.first.first=='C')
//            cout<<i.second.set_member.size()<<"gege";  // only one why?
        if(i.second.set_member.size()==1)
            output_info<<*i.second.set_member.begin()<<endl;
        else{
            int counter=0;
            output_info<<"{ ";
            for(auto j:i.second.set_member){
                output_info<<j;
                counter++;
                if(counter!=i.second.set_member.size())
                    output_info<<',';
            }
            output_info<<" }"<<endl;
        }
    }
}
void add_nfaToMovetable (char state_left,char vt,char state_right,NFA_movetable &move_matrix_NFA){
    pair<char,char> if_same(state_left,vt);
    // for(auto 插入失败)
    //  如move线性矩阵中没有相应表项
    nfa_move_item *temp=new nfa_move_item ;  // temp 为新表项
    NSET * tempset=new NSET ;
    bool if_stateleft_Final;
    if_stateleft_Final=tempset->add_one_destination(state_right);  //  跳转行为加入NSET
    if(if_stateleft_Final){
        nfa_final_state.insert(state_left);     // 如果是终态加入展示集合
//        cout<<state_left;
    }
    temp->first=if_same;
    temp->second=*tempset;
    move_matrix_NFA.insert(*temp);
}
void get_NFA_movetable_final(NFA_movetable &move_matrix_NFA){
    pair<char,char> bef,aft;
    for(auto i=move_matrix_NFA.begin();i!=move_matrix_NFA.end();i++){
        bef={i->first.first,i->first.second};
        nfa_move_item *temp=new nfa_move_item ;  // temp 为新表项
        NSET * tempset=new NSET ;
        tempset->set_member.insert(*i->second.set_member.begin());  // 连续表项的第一个先加入进来
        while(1){
            i++;
            if(i!=move_matrix_NFA.end()){   //  保证下一个状态还在move——matrix——NFA其中
                aft={i->first.first,i->first.second};
                if(aft==bef){
                    tempset->set_member.insert(*i->second.set_member.begin());
                }
                if(aft!=bef){       // 插入新集合
                    temp->first=bef;
                    temp->second=tempset;
                    move_matrix_NFA_final.insert(*temp);
                    i--;
                    break;
                }
            }
            else{
                i--;
                tempset->set_member.insert(*i->second.set_member.begin());
                temp->first=bef;
                temp->second=tempset;
                move_matrix_NFA_final.insert(*temp);
                break;
            }
        }
    }
    for(auto i:move_matrix_NFA_final){            // 变成map 求闭包查询速度大大提升
        turnto tt(i.first.first,i.first.second);
        move_matrix_NFA_MAP.insert((make_pair(tt,i.second.set_member)));
    }
    char before,after;
//    for(auto i=move_matrix_NFA_MAP.begin();i!=move_matrix_NFA_MAP.end();i++) {             // 递进处理生成useable_vt_for_NFA_VN
//        before=i->first.first;                                                          // 不可以根据move_matrix_nfa_final来遍历（红黑树本身无序）
//        set<char> *temp=new set<char>;
//        temp->insert(i->first.second);
//        auto bef_point=i;
//        while(1){
//            bef_point=i;    // unordered_map 不可通过减减后退
//            i++;
//            if(i!=move_matrix_NFA_MAP.end()){
//               after=i->first.first;
//               if(after==before){
//                   temp->insert(i->first.second);
//               }
//               else{
//                   useable_vt_for_NFA_VN.insert({before,*temp});
//                   i=bef_point;
//                   break;
//               }
//            }
//            else{
//                i=bef_point;
//                temp->insert(i->first.second);
//                useable_vt_for_NFA_VN.insert({before,*temp});
//                break;
//            }
//        }
//    }
    for(auto i=move_matrix_NFA.begin();i!=move_matrix_NFA.end();i++){
        before=i->first.first;
        set<char> *temp=new set<char>;
        temp->insert(i->first.second);
        while(1){
            i++;
            if(i!=move_matrix_NFA.end()){   //  保证下一个状态还在move——matrix——NFA其中
                after=i->first.first;
                if(after==before){
                    temp->insert(i->first.second);
                }
                if(after!=before){       // 插入新集合
                    useable_vt_for_NFA_VN.insert({before,*temp});
                    i--;
                    break;
                }
            }
            else{
                i--;
                temp->insert(i->first.second);
                useable_vt_for_NFA_VN.insert({before,*temp});
                break;
            }
        }
    }
}
void get_char_epsilion(char i,set<char> &newstate){
    turnto tt(i,'@');
    auto temp=move_matrix_NFA_MAP.find(tt);
//    for(auto i:temp->second)
//        cout<<i;
//    cout<<endl;

    if(temp==move_matrix_NFA_MAP.end())
        return ;
    else{
        for(auto k:temp->second){
            newstate.insert(k);
            get_char_epsilion(k,newstate);
        }
    }
}
void get_closure (NSET & nfa_set,NFA_movetable &move_matrix_NFA_final){       //    文法设计时就应该注意不能连续epsilon闭包多次，会导致循环深度不可预测，而递归调用可能会爆栈
// v1.0
//    NSET temp(nfa_set);
//    for(auto i:nfa_set.set_member){  // nfa_set中单个状态
//        for(auto j:move_matrix_NFA){
//            if(j.first.first==i&&j.first.second=='@'){
//                for(auto k:j.second.set_member){
//                    temp.set_member.insert(k);   // 第一层深度epsilon
//                    for(auto a:move_matrix_NFA){
//                        if(a.first.first==k&&a.first.second=='@'){
//                            for(auto b:a.second.set_member)
//                                temp.set_member.insert(b);  //  第二层深度epsilon
//                        }
//                    }
//                }
//            }
//        }
//    }

// v2.0
//    NSET temp(nfa_set);           // 任意深度
//    set<char> newstate;
//    for(auto i:temp.set_member){    // 对状态集中的每个元素
//        for(auto j:move_matrix_NFA_final){      // 在 move_final 表中查询@弧转化结果并加入temp，直到不再产生新状态
//            if(j.first.first==i&&j.first.second=='@'){
//                for(auto k:j.second.set_member){
//                    newstate.insert(k);
//                }
//            }
//        }
//    }
//    for(auto i:newstate)
//        temp.set_member.insert(i);
//    int bef=temp.set_member.size(),aft;
//    while(1){
//        for(auto i:newstate){    // 对状态集中的每个元素
//            for(auto j:move_matrix_NFA_final){      // 在 move_final 表中查询@弧转化结果并加入temp，直到不再产生新状态
//                if(j.first.first==i&&j.first.second=='@'){
//                    for(auto k:j.second.set_member){
//                        newstate.insert(k);
//                    }
//                }
//            }
//        }
//        aft=newstate.size();
//        if(aft!=bef)
//            bef=aft;
//        else
//            break;
//    }
//    nfa_set=temp;
//v3.0 通过map实现        递归实现，任意深度，速度加快
//    for(auto i:move_matrix_NFA_MAP){
//        turnto tt('S','@');
//        if(i.first.a=='S'&&i.first.b=='@'){
//            for(auto j:i.second)
//                cout<<j;
//            cout<<"above"<<endl;
//        }
//    }
//    for(auto i=move_matrix_NFA.begin();i!=move_matrix_NFA.end();++i){  // set本质是一棵树，要经过不可以直接对元组中的数值进行更改，必须先删除结点，自平衡后再插入
//        i = move_matrix_NFA.erase(i);
//    }
//    for(auto & i:nfa_set.set_member)
//        i.insert();
    set<char> newstate;
    for(auto i:nfa_set.set_member){
        auto point=VN_CLOSURE_SET.find(i);      // 先广度优先，找本状态集中可以出空的状态
        if(point!=VN_CLOSURE_SET.end()){
            for(auto it:point->second)
                newstate.insert(it);
        }
        else
            get_char_epsilion(i,newstate);  // 再对各个可以出空的状态进行深度优先，递归搜索
    }
    for(auto i:newstate)
        nfa_set.set_member.insert(i);
}
void add_NSET_toState (NSET &aft_set,NEW_of_OLDstate & O_IN_NEW,map<set<char>,int> &dfa_state_search){
    pair<int,set<char>> addin_temp(dfa_vn_counter,aft_set.set_member);
    DFA_VN.insert(dfa_vn_counter);          //  同时将终态放入全局 DFA 状态集中
    if(aft_set.set_member.find('$')!=aft_set.set_member.end()) //  如果确定化后的状态是终态
       dfa_final_state.insert({dfa_vn_counter,1});  //  是终结态度
    else
        dfa_final_state.insert({dfa_vn_counter,0});     // 非终结态
    O_IN_NEW.insert(addin_temp);
    dfa_state_search.insert({aft_set.set_member,dfa_vn_counter});   // dfa_state_search和O_IN_NEW同步更新
    dfa_vn_counter++;
}
void initialization (){
    VT_NUM=0;  //84  终结符个数不变
    NFA_VN_NUM=0;  //33
    DFA_VN_NUM=0;
    DFA_start_state=0;
    pu=0;             // 动态纪录epsilon闭包和a弧转化 次数
    error_number=0;
    final_bef='S',final_new='\0';
    VT.clear();  //NFA终结符
    VN.clear();  //NFA非终结符
    DFA_VN.clear(); // DFA非终结符  无效设计，set自排序可能不同状态开头代表元素也相同
    O_IN_NEW.clear();    // 真正的DFA非终结符结构设计
    nfa_final_state.clear();
    for_task2.clear(); // 根据字符号查询行号\位置; 字符index，<行号，字符>原本类型
    VN_CLOSURE_SET.clear();   // 终结符的闭包
    DFA_FINAL_STATE_FOR_PRINT.clear();  // DFA的终态map 数字号-原组成状态 由dfa_final_state 和 O_IN_NEW生成
    DFA_NOTFINAL_STATE_FOR_PRINT.clear();  // 同上
    dfa_state_search.clear();   // 辅助提供通过nfa状态集合寻找dfa状态的可能 和O_IN_NEW 互反
    useable_vt_for_NFA_VN.clear();     // 处理收集nfa中单个vn可用的vt
    dfa_state_ifcircleandepsilon.clear();   // 当前状态是否已经经过了i弧转换和词法语法分析
    dfa_final_state.clear();     // DFA的状态是否为终结态 （！！！！！！！！还是人为规定转换终态只要有$就算）  在O_IN_NEW中列出可以进入终态的所有状态
    dfa_vn_counter=0;          // 用于dfa状态计数插入 输出记得char int转换
    for_task2_num=1;
    line_index_for2=0;
    move_matrix_NFA.clear();          // for auto 只能遍历而不能插入
    move_matrix_NFA_final.clear();       // 真正的集合形式
    move_matrix_NFA_MAP.clear();        // 从final转化而来
    move_matrix_DFA.clear();
    move_matrix_DFA_FOR_SEARCH.clear();  // 由move_matrix_DFA演化而来，方便进行dfa寻路，词法分析

    source_code= fopen("../1_in_code.txt","r+");  // 打开要分析的源代码文件，用于scanner函数进行词法分析
    grammar_1=fopen("../1_in_grammar.txt","r+");              // 打开文法流 LexicalAnalysis_lhk
//    grammar_1=fopen("../1_test.txt","r+");
    output_string.open("../2_in_string.txt");
    output.open("../2_in_token.txt");               // 输出token表
    output_info.open("../info_about_task1.txt");    // 输出文法转化和token识别相关详细信息
}
void process_struct_foranalyse (){
    for(auto i:dfa_final_state){
        if(i.second==true){
           for(auto j:O_IN_NEW){
               if(j.first==i.first){
                   DFA_FINAL_STATE_FOR_PRINT.insert({i.first,j.second});
                   break;
               }
           }
        }
        else{
            for(auto j:O_IN_NEW){
                if(j.first==i.first){
                    DFA_NOTFINAL_STATE_FOR_PRINT.insert({i.first,j.second});
                    break;
                }
            }
        }
    }
    for(auto i:move_matrix_DFA){
        move_matrix_DFA_FOR_SEARCH.insert({i.first,i.second});
    }
}
void split (string line,vector<string>& words){
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
bool if_dfa (bool iffirstxiaoyu,bool iffirstdayu,int line_index,string word,DFA_movetable move_matrix_DFA,map<pair<int ,char>,int> DFA_SEARCH) {
    int pre=0;   // 转化后dfa状态
    // 如果已经到达一个终态，则不可再进行变更了(除非进行中可以变更后还是当前状态)
    final_bef='S',final_new='\0';
    map<pair<int ,char>,int>::iterator pointer;
    unordered_map<int,bool>::iterator if_in_final;   // 目标：获得新加入的终态
    set<char> bef_final,new_final,add_final;   // 上个状态的final 当前的final 新增的final
    output<<"       "<<line_index<<"        ";
    bool eight_flag=false;
    bool sci_flag=false;
    for (int i = 0; i < word.length(); ++i) {

        for (int i = 0; i < 50; ++i) {
            string k(keyword[i]);
            if(k==word){
                output<<setiosflags(ios::left)<<setw(16)<<"KEYWORD";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                string p("main"),q("return"),w("int"),e("float"),r("double"),y("complex"),c("continue"),b("break"),jj("while"),dd("for");
                if(word==p)
                    output_string<<'m';
                else if(word==q)
                    output_string<<'r';
                else if(word==w)
                    output_string<<'i';
                else if(word==e)
                    output_string<<'f';
                else if(word==r)
                    output_string<<'d';
                else if(word==y)
                    output_string<<'c';
                else if(word==c)
                    output_string<<'o';
                else if(word==b)
                    output_string<<'a';
                else if(word==jj)
                    output_string<<'w';
                else if(word==dd)
                    output_string<<'f';
                else
                    output_string<<'k';
                return true;
            }
        }
        if(word[0]=='0'&&word[1]>='0'&&word[1]<='9'){
            eight_flag=true;
            for (int j = 0; j < word.length(); ++j) {
                if(word[j]=='8'||word[j]=='9'){
                    error_number++;
                    cout<<error_number<<". ";
                    cout<<word<<" 常量(8进制) 定义错误 because of "<<word[j]<<endl;
                    output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                    output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                    return false;
                }
            }
        }

        if(word[0]>='0'&&word[0]<='9'&&word[i]=='E'){
            sci_flag=true;
            for (int j = i+1; j < word.length(); ++j) {
                if(word[j]=='.'){
                    error_number++;
                    cout<<error_number<<". ";
                    cout<<word<<" 常量(科学计数法) 定义错误 because of "<<word[j]<<endl;
                    output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                    output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                    return false;
                }
            }
        }
        bef_final.clear();
        new_final.clear();
        add_final.clear();
//        cout<<pre<<endl;
//        cout<<final_bef;
        pointer=DFA_SEARCH.find({pre,word[i]});   // pre(char word[i])->where (dfa state)
//        // 还得求一次闭包转化
//        pointer=DFA_SEARCH.find({pointer->second,'@'});
        if(dfa_final_state.find(pre)->second){    // 如果当前dfa状态为终态 ,对bef——final 操作
            for(auto ok:DFA_FINAL_STATE_FOR_PRINT.find(pre)->second){
                for(auto kk:nfa_final_state ){
                    if(kk==ok)
                        bef_final.insert(kk);
                }
            }
            if(bef_final.size()==1)  // 1.25E+4
                final_bef=*bef_final.begin();
        }
        if(pointer!=DFA_SEARCH.end()){  // 可进行 dfa move
           pre=pointer->second;          // 使用字符后当前状态号
           if_in_final=dfa_final_state.find(pre);
           if(if_in_final->second){  // 如果当前已经是dfa终结状态，找出对应nfa状态中的终结状态
               for(auto jj:DFA_FINAL_STATE_FOR_PRINT.find(pre)->second){  // 得到 new——final
                   for(auto kk:nfa_final_state ){     //  jj==kk nfa final
                       if(kk==jj)
                           new_final.insert(kk);
                   }
               }
               bool ifhave=false;
               for(auto ji:new_final){  // new_final
                   ifhave=false;
                   for(auto pq:bef_final){
                      if(ji==pq) ifhave=true;
                   }
                   if(!ifhave)
                       add_final.insert(ji);
               }

//               for(auto ooo:bef_final){
//                   cout<<ooo;
//               }
//               cout<<endl;
//               for(auto ooo:new_final){
//                   cout<<ooo;
//               }
//               cout<<endl;
//               for(auto ooo:add_final){
//                   cout<<ooo;
//               }
//               cout<<endl;

               if(add_final.size()==0)
                   continue;
               else{
                   final_new=*add_final.begin();
                   if(final_bef=='S'){
                       final_bef=final_new;
                       continue;
                   }
                   if(final_bef!='S'){
                       if(final_bef=='C'&&final_new=='F'){
                           final_bef=final_new;
//                           cout<<"have done F";
                       }
                       else if(final_bef=='C'&&final_new=='O'){
                           final_bef=final_new;
                       }
                       else if(final_bef=='C'&&final_new=='P'){
                           final_bef=final_new;
                       }
                       else if(final_bef=='C'&&final_new=='E'){
                           final_bef=final_new;
                       }
                       else if(final_bef=='F'&&final_new=='O'){ //1.25E+4
                           final_bef=final_new;
                       }
                       else if(final_bef=='F'&&final_new=='E'){ //2+3.7I
                           final_bef=final_new;
                       }
                       // reverse
                       else if(final_bef=='C'&&final_new=='K'){  // 2+3.7I
                           final_new=final_bef;
                       }
                       else if(final_bef=='I'&&final_new=='C'){ //_a12
                           final_new=final_bef;         //  !!
                       }
                       else if(final_bef=='F'&&final_new=='K'){ //1.25E+4
                           final_new=final_bef;         //  !!
                       }
//                       else if(final_bef=='I'&&final_new=='K'){
//                           final_new=final_bef;
//                       }
//                       else if(final_bef=='K'&&final_new=='C'){
//                           final_new=final_bef;
//                       }
                       else{   // 出现错误
                           if(final_bef=='I'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" 标识符 定义错误 because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='C'&&final_new=='I'){   //  用于 000aaa
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" 标识符 定义错误 because of "<<word[i-1]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='C'){
                               error_number++;
                               cout<<error_number<<". ";
//                               cout<<final_bef<<final_new;
                               cout<<word<<" 常量 定义错误 because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='F'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" 常量(浮点数) 定义错误 because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='O'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" 常量(科学计数法) 定义错误 because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='D'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" 常量(16进制数) 定义错误 because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='E'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" 常量(复数) 定义错误 because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='K'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" 运算符||限定符 定义错误 because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                           output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                       }
//                       cout<<endl<<final_bef<<word[i]<<endl;
                   }
               }
//               if(add_final.size()>=2){             // 验证文法每一步新增状态不能超过一个 明显可以
//                   cout<<"???";
//                   for(auto ou:add_final)
//                       cout<<ou;
//                   cout<<endl;
//               }
           }
        }
        else{
            err :
            if(final_bef=='I'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" 标识符 定义错误 because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='C'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" 常量 定义错误 because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='F'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" 常量(浮点数) 定义错误 because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='O'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" 常量(科学计数法) 定义错误 because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='P'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" 常量(16进制数) 定义错误 because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='E'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" 常量(复数) 定义错误 because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='K'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" 运算符||限定符 定义错误 because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            error_number++;
            cout<<error_number<<". ";
            cout<<word<<" 定义错误 because of "<<word[i]<<endl;
            output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
            output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
           return false;
        }
    }
    if(final_new=='I'){
        output<<setiosflags(ios::left)<<setw(16)<<"IDENTIFIER";
        output_string<<'e';
    }
    if(final_new=='C'){
        if(word[0]=='0'){
            output<<setiosflags(ios::left)<<setw(16)<<"CONSTANT(8)";
            output_string<<'2';
        }
        else{
            output<<setiosflags(ios::left)<<setw(16)<<"CONSTANT(10)";
            output_string<<'1';
        }
    }
    if(final_new=='F'){
        output<<setiosflags(ios::left)<<setw(16)<<"CONSTANT(flo)";
        output_string<<'5';
    }
    if(final_new=='O'){
        output<<setiosflags(ios::left)<<setw(16)<<"CONSTANT(sci)";
        output_string<<'4';
    }
    if(final_new=='P'){
        output<<setiosflags(ios::left)<<setw(16)<<"CONSTANT(16)";
        output_string<<'3';
    }
    if(final_new=='E'){
        output<<setiosflags(ios::left)<<setw(16)<<"CONSTANT(com)";
        output_string<<'6';
    }
    if(final_new=='K'){
        if(word.size()==2)
            output_string<<'b';
        else{
            if(word[0]==';'||word[0]=='#'||word[0]=='('||word[0]==')'||word[0]=='{'||word[0]=='}'||word[0]=='<'||word[0]=='>'||word[0]=='=')
                output_string<<word[0];
            else
                output_string<<'s';
        }
        if(word[0]==','||word[0]=='('||word[0]==')'||word[0]=='{'||word[0]=='}'||word[0]==';'||word[0]==':'||word[0]=='#')
        output<<setiosflags(ios::left)<<setw(16)<<"DELIMITER";
        else if(word[0]=='<'&&iffirstxiaoyu){
            output<<setiosflags(ios::left)<<setw(16)<<"DELIMITER";
            iffirstxiaoyu=false;
        }
        else if(word[0]=='>'&&iffirstdayu){
            output<<setiosflags(ios::left)<<setw(16)<<"DELIMITER";
            iffirstdayu= false;
        }
        else{
            output<<setiosflags(ios::left)<<setw(16)<<"OPERATOR";
        }
    }
    output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
    return true;
}
void Grammar_to_NFA (){
    int n;
    char line[100];             //   使用fgets必须先分配内存，不可空指针
//    line.resize(7);  // 分配内存后才能用fscanf 尽量不用resize(变capacity)
    char state_left,state_right;
    char vt;
    fseek(grammar_1,0,SEEK_SET);
    fgets(line,5,grammar_1);
    string numofline(line);
    n= stoi(numofline);
    output_info<<"                                   词法分析文法 相关信息统计(已排序处理)  "<<endl;
    output_info<<"正规式条数 : "<<n<<endl;
    _for(i,0,n){                            // less ABKY IY
//        fscanf(grammar_1,"%s",&line[0]);
        fgets(line,100,grammar_1);
        state_left=line[0];
        vt=line[3];
        if(i==0){               // 初态设置
            start_state=state_left;
        }
        if(!if_inVN(state_left,VN))  // 对左部进行添加
            VN.insert(state_left);
        if(!if_inVT(vt,VT))     // 对右部非终结符进行添加
            VT.insert(vt);
        if(((int)line[4]>=(int)'A'&&(int)line[4]<=(int)'Z')||((int)line[4]>=(int)'a'&&(int)line[4]<=(int)'f')){                // 右部有VN 对move矩阵添加表项
            state_right=line[4];
        }
        else{                       // 无VN 对左边部分添加 (vt)->#(终态)
            state_right='#';
        }
        add_nfaToMovetable(state_left,vt,state_right,move_matrix_NFA); //  添加nfa move表项
    }
    get_NFA_movetable_final(move_matrix_NFA);
    NFA_VN_NUM=VN.size();
    VT_NUM=VT.size();
    output_info<<"Vn数目 : "<<NFA_VN_NUM<<endl;
    output_info<<"        { ";
    for(auto i:VN)
        output_info<<i;
    output_info<<" }"<<endl;
    output_info<<"Vt数目 : "<<VT_NUM<<endl;
    output_info<<"        { ";
    for(auto i:VT)
        output_info<<i;
    output_info<<" }"<<endl;
    output_info<<"初态 : "<<start_state<<endl;
    output_info<<"NFA终态数目 : "<<nfa_final_state.size()<<endl;
    output_info<<"        { ";
    for(auto i:nfa_final_state)
        output_info<<i;
    output_info<<" }"<<endl;
    show_NFA_movetable(move_matrix_NFA_final);
}
void NFA_to_DFA (){
    NSET pre_set;
    pre_set.set_member.insert(start_state);     // 先处理初态
    stack<NSET> s;
    get_closure(pre_set,move_matrix_NFA);  // !!!!!!!wait for write  如遇到->@ 后集合中有$
    s.push(pre_set);
//    for(auto i:pre_set.set_member)
//        cout<<i;
//    cout<<endl;
    DFA_start_state=0;   // 0状态是起始状态
    add_NSET_toState(pre_set,O_IN_NEW,dfa_state_search); // 得到旧状态组成的新状态结构体，方便后续操作     (将NSET变成NEW_of_OLDstate中的元组)
    pu=1;  // 当前行第几个
    set<char> temp_allA_inSET;     // 状态集合中所有状态可以进行的a集合(剪枝，大幅优化计算时间，对多状态计算效果显著)
    map<set<char>,int> cut_tree;   //  同一行的如新增状态相同，则不用再二次取闭包并添加状态了，直接退出即可
    set<char> add_state;        // 当前行状态经过i弧转化后新增的状态
    map<char,char> for_quickcompare;
    map<int,bool> have_gothrough_line_states;  // 一个状态是否已经走过完整的一行了
    have_gothrough_line_states.clear();

    int counter=0;

    while(!s.empty()){

        pre_set=s.top();
        s.pop();
        int bef= if_inNewSet(pre_set,dfa_state_search); // 一行的起始状态不变
        if(have_gothrough_line_states.find(bef)!=have_gothrough_line_states.end())
            continue;
        pu=1;

        for_quickcompare.clear();
        for(auto i:pre_set.set_member)    // 用于新增状态解法优化
            for_quickcompare.insert({i,i});

        temp_allA_inSET.clear();
        for (auto i:pre_set.set_member) {   // 对pre_set状态集合中每个状态都进行a弧转换并取闭包
            auto jj=useable_vt_for_NFA_VN.find(i);
            if(jj!=useable_vt_for_NFA_VN.end()){
                for(auto j:useable_vt_for_NFA_VN.find(i)->second)
                    temp_allA_inSET.insert(j);
            }
            else{
//                goto here2;
                continue;
            }
        }
//  已取得可能使用的a弧集合
        // 对已经缩小的可能发生状态转移的a弧集合，查询pre_set所有状态，看是否能转换，能则加入其子集

        cut_tree.clear();

        for (auto i:temp_allA_inSET) {          // 进行 i 弧转换
            add_state.clear();
            NSET pre_set_copy(pre_set);     // !!!!注意每做一次闭包以及转换之前对pre_set复制拷贝
            for (auto j:pre_set.set_member) {   // j 状态集中状态个数
                pair<char,char> match_move(j,i);    // j状态进行i弧转换
                auto mm=move_matrix_NFA_MAP.find(match_move);   //  查找到转换后新增状态
                if(mm!=move_matrix_NFA_MAP.end()){
                    pre_set_copy.set_member.insert(mm->second.begin(),mm->second.end());
//                    add_state.insert(mm->second.begin(),mm->second.end());
                    for(auto kkl:mm->second){    // 真正正确的新增状态求法
                       auto pp=for_quickcompare.find(kkl);
                       if(pp==for_quickcompare.end()){
                           add_state.insert(kkl);
                       }
                    }
                }
                else
                    continue;
            }
            map<set<char>,int>::iterator ii;
            auto temppointer=cut_tree.find(add_state);
            int aft,num;
            dfa_move_item tempformoveitem;
            if(temppointer!=cut_tree.end()&&pu!=1) {  //  对当前状态再求闭包并添加压入状态是多余的
                goto here1;        // goto 之后不可再声明变量
            }
//            if(i=='!'){
//                for(auto p:pre_set_copy.set_member)
//                    cout<<p;
//                cout<<1<<endl;
//            }

            get_closure(pre_set_copy,move_matrix_NFA_final);  //  !!!wait for write!!!!!!!
            counter++;
            pu++;
//            if(i=='!'){
//                for(auto p:pre_set_copy.set_member)
//                    cout<<p;
//                cout<<2<<endl;
//            }

            num=if_inNewSet(pre_set_copy,dfa_state_search);   //  !!!wait for change!!!!!
            if(num==-1){             //  当前状态为新状态
//                for(auto jjo:pre_set_copy.set_member){
//                    if(jjo=='$')    // 已经是终结态了，不用再进行push求行闭包转化了
//                      goto jjj;
//                }
                s.push(pre_set_copy);
            jjj:
                add_NSET_toState(pre_set_copy,O_IN_NEW,dfa_state_search);
                cut_tree.insert({add_state,dfa_vn_counter-1});
            }
            aft= if_inNewSet(pre_set_copy,dfa_state_search);
            tempformoveitem={{bef,i},aft};
            move_matrix_DFA.insert(tempformoveitem); //不论是不是新状态都要对dfa——move进行添加
            goto havedone;
    here1 :
            ii=cut_tree.find(add_state);
            tempformoveitem={{bef,i},ii->second};
            move_matrix_DFA.insert(tempformoveitem);
     havedone :
            ;
        }
        have_gothrough_line_states.insert({bef,1});
//        cout<<"have done "<<counter<<"times , "<<"have "<<O_IN_NEW.size()<<" in new DFA state "<<endl;
//        for(auto q:O_IN_NEW){
//            cout<<q.first<<" ";
//            for(auto w:q.second)
//                cout<<w;
//        }
          // stack and number
//        cout<<s.size()<<" "<<O_IN_NEW.size();
//        cout<<endl;

//    here2 :
//        ;
    }
    DFA_VN_NUM=O_IN_NEW.size();
}
void show_MoveMatrix (){
    //  输出信息到output_info
    output_info<<""<<endl;
    output_info<<"                                   NFA -> DFA Convert ( NFA 确定化结果 )  "<<endl;
    output_info<<"                     经过 "<<pu<<" 次有效的epsilon闭包和a弧转化,将 "<<NFA_VN_NUM<<" 个NFA状态集转化为 "<<DFA_VN_NUM<<" 个DFA状态"<<endl;
    output_info<<"Vn数目 : "<<DFA_VN_NUM<<" ( 状态用数字表示 )"<<endl;
    output_info<<"        { ";
    int counter=0;
    for(auto i=O_IN_NEW.begin();i!=O_IN_NEW.end();i++){
        i++;
        if(i!=O_IN_NEW.end()){
            i--;
            output_info<<i->first<<',';
            counter++;
        }
        else{
            i--;
            output_info<<i->first;
            counter++;
        }
        if(counter==36){
            output_info<<endl<<"          ";
            counter=0;
        }
    }
    output_info<<" }"<<endl;
    output_info<<"Vt数目 : "<<VT_NUM<<endl;
    output_info<<"        { ";
    for(auto i:VT)
        output_info<<i;
    output_info<<" }"<<endl;
    output_info<<"初态 : "<<DFA_start_state<<" { ";
    for(auto i:O_IN_NEW){
        if(i.first==0){
            int temp=i.second.size();
            for(auto j:i.second){
                temp--;
                if(temp>=1)
                    output_info<<j<<',';
                else
                    output_info<<j;
            }
            output_info<<" }"<<endl;
            break;
        }
    }
    process_struct_foranalyse();
    output_info<<"DFA终态数目 : "<<DFA_FINAL_STATE_FOR_PRINT.size()<<" 按 “ 状态 { 组成该DFA状态的NFA状态集合 } ” 的形式输出如下 :"<<endl;
    temp_for_print just_temp_inorderforprint;
    for(auto i:dfa_final_state)
        just_temp_inorderforprint.insert({i.first,i.second});
    for(auto i:just_temp_inorderforprint){
        if(i.second==true){
            output_info<<"             "<<i.first<<" { ";
            auto j=DFA_FINAL_STATE_FOR_PRINT.find(i.first);
            int temp=j->second.size();
            for(auto k:j->second){
                temp--;
                if(temp>=1)
                    output_info<<k<<',';
                else
                    output_info<<k;
            }
            output_info<<" }"<<endl;
        }
    }
    output_info<<"                                   DFA 状态转移矩阵   "<<endl;
    output_info<<"          Vn_SOURCE              Vn               Vn.DESTINATION "<<endl;
    for(auto i:move_matrix_DFA){
        auto it=DFA_FINAL_STATE_FOR_PRINT.find(i.first.first);
        output_info<<"             "<<i.first.first;
        if(it!=DFA_FINAL_STATE_FOR_PRINT.end())
            output_info<<" ( final )   ";
        output_info<<"                   "<<i.first.second<<"                    "<<i.second;
        it=DFA_FINAL_STATE_FOR_PRINT.find(i.second);
        if(it!=DFA_FINAL_STATE_FOR_PRINT.end())
            output_info<<" ( final )   ";
        output_info<<endl;
    }

}
void scanner (){
    ifstream source("../1_in_code.txt");
    string line;
    vector<string> words;
    map<pair<int ,char>,int> DFA_SEARCH;  // 方便dfa查询
    bool iffirstxiao=true,iffirstdayu=true;
    DFA_SEARCH.clear();
    for(auto ok:move_matrix_DFA)
        DFA_SEARCH.insert({ok.first,ok.second});
    output<<"                           TOKEN TABLE"<<endl;
    output<<"       line        "<<" type        "<<"        word        "<<endl;
//    while(getline(source,line)){
//        for(auto i:line)
//            cout<<i<<' ';
//        cout<<endl;
//    }
    cout<<endl<<"      task 1 is running ... "<<endl<<endl;
    cout<<"源代码分析结果如下所示 :"<<endl;

    while(getline(source,line)) {
        line_index_for2++;
        words.clear();
        split(line,words);
//        for(auto i:words){
//            cout<<i<<" ";
//        }
//        cout<<endl;
        for(auto i:words){
            if_dfa(iffirstxiao,iffirstdayu,line_index_for2,i,move_matrix_DFA,DFA_SEARCH);
            for_task2.insert({for_task2_num++,{{line_index_for2,i},final_bef}});
//            cout<<i<<" done dfa"<<endl;
        }
    }
    output_string.close();
    if(!error_number)
        cout<<endl<<"SUCCESS----文件中源代码符合文法规范";
}
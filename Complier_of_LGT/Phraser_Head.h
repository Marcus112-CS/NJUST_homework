//
// Created by DELL on 2022/4/26.
//

#ifndef COMPLIER_OF_LGT_PHRASER_HEAD_H
#define COMPLIER_OF_LGT_PHRASER_HEAD_H
#include <bits/stdc++.h>
using namespace std;

//      结构体及类型定义
typedef struct right_source{  // 用于储存产生式右部
    vector<char>  right_inorder;
    string right_for_compare;
    bool if_a_single_epsilon=false;
    right_source(){};
    right_source(string right){    // 传入产生式自动生成其right_source;
        string temp=right.substr(3);
        this->right_for_compare=temp;
        for (int i = 0; i < temp.length(); ++i) {
            this->right_inorder.push_back(temp[i]);
            if(temp[i]=='@')
                this->if_a_single_epsilon=true;
        }
    }
}rs;               // 用于储存产生式右部

typedef struct project_set_item {
    int line_index;
    char left_Vn;
    vector<char> right;
    char tail;
    bool operator==(const project_set_item & it)const  // 运算符重载
    {  // line .position tail 都一致
        if(this->line_index==it.line_index&&this->tail==it.tail){
            int counter_1=-1;
            for(auto i:it.right){
                counter_1++;
                if(i=='.')
                    break;
            }
            int counter_2=-1;
            for(auto i:this->right){
                counter_2++;
                if(i=='.')
                    break;
            }
            if(counter_1==counter_2)
                return true;
            else
                return false;
        }
        return false;
    }
    project_set_item(){};
    project_set_item(project_set_item *pre){  // 拷贝构造
        this->line_index=pre->line_index;
        this->left_Vn=pre->left_Vn;
        for(auto i:pre->right)
            this->right.push_back(i);
        this->tail=pre->tail;
    }
}project_set_ITEM;    //项目集中的单条产生式（包含各种接收和tail信息） 可以直接==判等
typedef struct action_item{
    int movement=0; // 状态 0 非法 1 S 2 r 3 acc
    int number=0;   // 配合movement形成具体含义
    action_item(){}
    action_item(char movement_first_char,int number){
        if(movement_first_char=='S'||movement_first_char=='s')
            this->movement=1;
        if(movement_first_char=='R'||movement_first_char=='r')
            this->movement=2;
        if(movement_first_char=='A'||movement_first_char=='a')
            this->movement=3;
        this->number=number;
    }
    bool operator== (const struct action_item &a)const
    {
        if(movement==a.movement&&number==a.number)
            return true;
        else
            return false;
    }
    string movement_chinese (){
        string res;
        if(movement==1){
            res="移进";
        }
        if(movement==2){
            res="规约";
        }
        if(movement==3)res="接受(accepted)";
        return res;
    }
    string get_output_string(){
        string res;
        if(movement==1){
           res+='S';
           res+= to_string(this->number);
        }
        if(movement==2){
            res+='r';
            res+= to_string(this->number);
        }
        if(movement==3)res+="acc";
        return res;
    }
}ACTION_item;  // 用于ACTION表的构建second
struct compare{
    bool operator()(const rs &a,const rs &b)const{
        if(a.right_for_compare<b.right_for_compare)
            return true;
        else
            return true;
    }
    bool operator()(const project_set_ITEM &a,const project_set_ITEM &b)const{  // 保证绝对弱序才能去重！！！！重要
        if(a.line_index!=b.line_index)
            return a.line_index<b.line_index;
        else{                         // 重载缺失导致无法插入新表项
            if(a.tail!=b.tail)
                return a.tail<b.tail;
            else{
                int temp_1=0,temp_2=0;
                for(auto i:a.right){
                    temp_1++;
                    if(i=='.')
                        break;
                }
                for(auto i:b.right){
                    temp_2++;
                    if(i=='.')
                        break;
                }
                return temp_1<temp_2;
            }
        }
    }
    bool operator()(const pair<pair<int,char>,int> &a,const pair<pair<int,char>,int> &b)const{
        if(a.first.first<b.first.first)return true;
        else if(a.first.first==b.first.first){
            if(a.first.second<=b.first.second)return true;
            else return false;
        }
        else return false;
    }
    bool operator()(const pair<pair<int,char>,ACTION_item> &a,const pair<pair<int,char>,ACTION_item> &b)const{
        if(a.first.first<b.first.first)return true;
        else if(a.first.first==b.first.first){
            if(a.first.second<=b.first.second)return true;
            else return false;
        }
        else return false;
    }
};


typedef set<rs,compare> RIGHT_SET;  // 存放一个vn对应所有的右部
typedef set<project_set_ITEM,compare> PROJECT_SET; // 项目集  项目默认按照输入时的顺序进行排序  使用if_equal判等


//             Phraser_Function.cpp
// 原子功能函数
void split_2 (string line,vector<string> &words); // 分隔读取，用于将token序列转换成所需的单行待规约串(init中)
void init ();       // 数据初始化
void GOTO_ACTION_TABLE_show ();
void get_stack_char_vector_version (stack<char> character,vector<char> &character_fornow);   // 将stack变为vector方便操作
void get_stack_state_vector_version (stack<int> state,vector<int> &state_fornow);
bool if_equal (PROJECT_SET a,PROJECT_SET b);     // 判断两个项目集是否相等
bool if_is_new_ACTION_item (pair<pair<int,char>,ACTION_item> temp);   // 用于去重
int if_in_new_state (PROJECT_SET aft_set);      // 判断一个集合是否在新状态中，如果否，则返回状态号，是则返回-1
set<char> get_tail (string temp);         // 对temp求first集合以获得新tail集
project_set_ITEM init_project_set_item (int line_index);     // 根据行号进行初始化
PROJECT_SET GOTO_SET (PROJECT_SET pre_set_copy,char v);  // 使用字符v进行GOTO操作的时候得到的初步项目集

// 组合功能函数
void vn_epsilon_process ();     // 根据task2文法处理vn判断是否可以推出epsilon
void vn_get_FIRST_SET ();       // 求取vt的first集
void get_CLOSURE (PROJECT_SET & bef); // 对一个项目集合求产生式闭包，并对可能的item刷新tail值
void LR_1_CREATE ();        // 重要函数，用于求取LR项目集合的同时对 新状态转换表进行添加  ！！！ 增广文法 $->S
void get_ACTION_and_GOTO ();  // 统计得到ACTION和GOTO表
void LR_1_ANALYSIS ();   // LR(1)分析，扫描返回结果

//              Phraser_Analyze.cpp
void Phraser_Analyze();
#endif //COMPLIER_OF_LGT_PHRASER_HEAD_H

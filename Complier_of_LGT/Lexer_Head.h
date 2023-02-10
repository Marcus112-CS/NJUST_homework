//
// Created by DELL on 2022/4/26.
//

#ifndef COMPLIER_OF_LGT_LEXER_HEAD_H
#define COMPLIER_OF_LGT_LEXER_HEAD_H
#include <bits/stdc++.h>
#define _for(i,a,b) for(int i=a;i<b;i++)
using namespace std;

//      结构体及类型定义
typedef struct NFASET{        // NFA 状态集 || 可用于组成nfamove矩阵的item(if_finalstate无效)
    set<char> set_member;
    NFASET(){};
    NFASET(NFASET* pre){      //  2. 用于nfa->dfa
        this->set_member.clear();
        for(auto i:pre->set_member)
            this->set_member.insert(i);
    };
    bool add_one_destination(char state_right){    //1.用于文法转nfa 将当前move加入NSET同时返回state_left是否为终结态
        if(state_right=='#'){                             //  ->(a)'0'    终态   (将->@and->vt同等看待)
            this->set_member.insert('$');              //       !!!! $ 代表当前状态可以为最终态
            return true;
        }
        else{
            this->set_member.insert(state_right);
            return false;
        }
    }
}NSET;            // NFA 状态集 || 可用于组成nfamove矩阵的item(if_finalstate无效)
using turnto = std::pair<char,char>;
namespace std {
    template<> struct hash<pair<char, char>> {
        inline size_t operator()(const pair<char, char> v) const {
            hash<char> h;
            return (h(v.first) << 32) + h(v.second);
        }
    };
}


typedef pair<pair<char,char>,NSET> nfa_move_item;   //    NFA_movetable 中的单元    根据pair<状态,VT> 状态(VT)->NSET(状态集)
typedef pair<pair<int,char>,int> dfa_move_item;
struct cmp{
    bool operator()(const nfa_move_item &a,const nfa_move_item &b)const{
        if(a.first.first<b.first.first) return true;
        else if(a.first.first==b.first.first){
            if(a.first.second<=b.first.second) return true;
            else return false;
        }
        else return false;
    }
    bool operator()(const dfa_move_item &a,const dfa_move_item &b)const{
        if(a.first.first<b.first.first) return true;
        else if(a.first.first==b.first.first){
            if(a.first.second<=b.first.second) return true;
            else return false;
        }
        else return false;
    }
    bool operator()(const pair<int,set<char>> &a,const pair<int,set<char>> &b)const{
        return a.first<=b.first;
    }
    bool operator()(const pair<int,bool> &a,const pair<int,bool> &b)const{
        return a.first<=b.first;
    }
//    bool operator()(const TURNTO &a,const TURNTO &b)const{
//        if(a.a<b.a)return true;
//        else if(a.a==b.a){
//            if(a.b<=b.b)return true;
//            else return false;
//        }
//        else return false;
//    }
};                          // 结构体比较函数——用于set去重

typedef set<pair<int,bool>,cmp> temp_for_print;          // 用于有序输出
typedef set<nfa_move_item ,cmp> NFA_movetable;
typedef set<dfa_move_item ,cmp> DFA_movetable;
typedef set<pair<int,set<char>> ,cmp> NEW_of_OLDstate;     //  纪录新状态的组成部分(旧状态)  first 不能取char（不能保证唯一性）
typedef unordered_map<turnto,set<char>> SEARCH_NFA;      // 将move_nfa_matrix_final 转化成可以用下标访问的结构

//             Lexer_Function.cpp
// 原子功能函数
bool if_inVT (char a,set<char> VT);                    // 是否为终结符
bool if_inVN (char a,set<char> VN);                    // 是否为非终结符
int if_inNewSet (NSET & pre_set_copy,map<set<char>,int> dfa_state_search);      // 当前a弧转换求闭包后的状态是否已经在新DFA状态的集合中
void show_NFA_movetable(NFA_movetable move_matrix_NFA_final);   // 在文法信息中打印NFA 线性move矩阵
void add_nfaToMovetable (char state_left,char vt,char state_right,NFA_movetable &move_matrix_NFA); //  将单条正规式加入NFA_movetable结构中
void get_NFA_movetable_final(NFA_movetable& move_matrix_NFA);        // 将每条单个状态的move_matrix_NFA变final
void get_char_epsilion(char i,set<char> &newstate);                              // 递归求取闭包为上方函数原子函数
void get_closure (NSET & nfa_set,NFA_movetable& move_matrix_NFA_final);          // 对状态取 epsilon(@)闭包
void add_NSET_toState (NSET &aft_set,NEW_of_OLDstate & O_IN_NEW,map<set<char>,int> &dfa_state_search);  // 将NSET(DFA初步确定化后的状态集)变成新状态结构体,方便排序统计和新操作
void initialization ();             // 数据初始化
void process_struct_foranalyse ();      // 处理已有数据结构生成便于查询的结构方便scan和dfa
void split (string line,vector<string>& words);   // 将一行source code 拆分
bool if_dfa (bool iffirstxiaoyu,bool iffirstdayu,int line_index,string word,DFA_movetable move_matrix_DFA,map<pair<int ,char>,int> DFA_SEARCH) ;  // 是否可以走通dfa路径
// 组合功能函数
void Grammar_to_NFA ();             // 3型文法 -> NFA (状态转移矩阵生成)  line 11、12
void NFA_to_DFA ();                 // NFA 确定化 生成新状态转移矩阵
void show_MoveMatrix ();            // 打印状态转移矩阵
void scanner ();                    // 扫描代码源文件进行词法分析,生成token表放入2_in_grammar.txt

//              Lexer_Analyze.cpp
void Lexer_Analyze ();              // 词法分析总入口
#endif //COMPLIER_OF_LGT_LEXER_HEAD_H

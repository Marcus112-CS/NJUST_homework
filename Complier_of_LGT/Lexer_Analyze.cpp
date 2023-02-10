//
// Created by DELL on 2022/4/26.
//
#include <bits/stdc++.h>
#include "Lexer_Head.h"
using namespace std;

extern int VT_NUM;  //84
extern int NFA_VN_NUM;  //33
extern int DFA_VN_NUM;
extern char start_state;
extern int DFA_start_state;
extern int pu;
extern set<char> VT;  //终结符
extern set<char> VN;  //非终结符
extern set<int> DFA_VN;
extern set<char> nfa_final_state;
extern unordered_map<char,set<char>> VN_CLOSURE_SET;
extern unordered_map<int ,set<char>> DFA_FINAL_STATE_FOR_PRINT;  //
extern unordered_map<int ,set<char>> DFA_NOTFINAL_STATE_FOR_PRINT;
extern map<set<char>,int> dfa_state_search;
extern map<int,pair<pair<int,string>,char>> for_task2; // 根据字符号查询行号\位置;
extern unordered_map<char,set<char>> useable_vt_for_NFA_VN;
extern unordered_map<int,bool> dfa_state_ifcircleandepsilon;
extern unordered_map<int,bool> dfa_final_state;
extern int dfa_vn_counter;
extern NEW_of_OLDstate O_IN_NEW;
extern NFA_movetable move_matrix_NFA;
extern DFA_movetable move_matrix_DFA;
extern NFA_movetable move_matrix_NFA_final;
extern SEARCH_NFA  move_matrix_NFA_MAP;

extern FILE* source_code;
extern FILE* grammar_1;
extern ofstream output_info;
extern ofstream output;
extern ofstream output_string;

void Lexer_Analyze (){
    initialization();
    Grammar_to_NFA();
    NFA_to_DFA();
    show_MoveMatrix();
    scanner();
}
//
// Created by DELL on 2022/4/26.
//

#ifndef COMPLIER_OF_LGT_PHRASER_HEAD_H
#define COMPLIER_OF_LGT_PHRASER_HEAD_H
#include <bits/stdc++.h>
using namespace std;

//      �ṹ�弰���Ͷ���
typedef struct right_source{  // ���ڴ������ʽ�Ҳ�
    vector<char>  right_inorder;
    string right_for_compare;
    bool if_a_single_epsilon=false;
    right_source(){};
    right_source(string right){    // �������ʽ�Զ�������right_source;
        string temp=right.substr(3);
        this->right_for_compare=temp;
        for (int i = 0; i < temp.length(); ++i) {
            this->right_inorder.push_back(temp[i]);
            if(temp[i]=='@')
                this->if_a_single_epsilon=true;
        }
    }
}rs;               // ���ڴ������ʽ�Ҳ�

typedef struct project_set_item {
    int line_index;
    char left_Vn;
    vector<char> right;
    char tail;
    bool operator==(const project_set_item & it)const  // ���������
    {  // line .position tail ��һ��
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
    project_set_item(project_set_item *pre){  // ��������
        this->line_index=pre->line_index;
        this->left_Vn=pre->left_Vn;
        for(auto i:pre->right)
            this->right.push_back(i);
        this->tail=pre->tail;
    }
}project_set_ITEM;    //��Ŀ���еĵ�������ʽ���������ֽ��պ�tail��Ϣ�� ����ֱ��==�е�
typedef struct action_item{
    int movement=0; // ״̬ 0 �Ƿ� 1 S 2 r 3 acc
    int number=0;   // ���movement�γɾ��庬��
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
            res="�ƽ�";
        }
        if(movement==2){
            res="��Լ";
        }
        if(movement==3)res="����(accepted)";
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
}ACTION_item;  // ����ACTION��Ĺ���second
struct compare{
    bool operator()(const rs &a,const rs &b)const{
        if(a.right_for_compare<b.right_for_compare)
            return true;
        else
            return true;
    }
    bool operator()(const project_set_ITEM &a,const project_set_ITEM &b)const{  // ��֤�����������ȥ�أ���������Ҫ
        if(a.line_index!=b.line_index)
            return a.line_index<b.line_index;
        else{                         // ����ȱʧ�����޷������±���
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


typedef set<rs,compare> RIGHT_SET;  // ���һ��vn��Ӧ���е��Ҳ�
typedef set<project_set_ITEM,compare> PROJECT_SET; // ��Ŀ��  ��ĿĬ�ϰ�������ʱ��˳���������  ʹ��if_equal�е�


//             Phraser_Function.cpp
// ԭ�ӹ��ܺ���
void split_2 (string line,vector<string> &words); // �ָ���ȡ�����ڽ�token����ת��������ĵ��д���Լ��(init��)
void init ();       // ���ݳ�ʼ��
void GOTO_ACTION_TABLE_show ();
void get_stack_char_vector_version (stack<char> character,vector<char> &character_fornow);   // ��stack��Ϊvector�������
void get_stack_state_vector_version (stack<int> state,vector<int> &state_fornow);
bool if_equal (PROJECT_SET a,PROJECT_SET b);     // �ж�������Ŀ���Ƿ����
bool if_is_new_ACTION_item (pair<pair<int,char>,ACTION_item> temp);   // ����ȥ��
int if_in_new_state (PROJECT_SET aft_set);      // �ж�һ�������Ƿ�����״̬�У�������򷵻�״̬�ţ����򷵻�-1
set<char> get_tail (string temp);         // ��temp��first�����Ի����tail��
project_set_ITEM init_project_set_item (int line_index);     // �����кŽ��г�ʼ��
PROJECT_SET GOTO_SET (PROJECT_SET pre_set_copy,char v);  // ʹ���ַ�v����GOTO������ʱ��õ��ĳ�����Ŀ��

// ��Ϲ��ܺ���
void vn_epsilon_process ();     // ����task2�ķ�����vn�ж��Ƿ�����Ƴ�epsilon
void vn_get_FIRST_SET ();       // ��ȡvt��first��
void get_CLOSURE (PROJECT_SET & bef); // ��һ����Ŀ���������ʽ�հ������Կ��ܵ�itemˢ��tailֵ
void LR_1_CREATE ();        // ��Ҫ������������ȡLR��Ŀ���ϵ�ͬʱ�� ��״̬ת����������  ������ �����ķ� $->S
void get_ACTION_and_GOTO ();  // ͳ�Ƶõ�ACTION��GOTO��
void LR_1_ANALYSIS ();   // LR(1)������ɨ�践�ؽ��

//              Phraser_Analyze.cpp
void Phraser_Analyze();
#endif //COMPLIER_OF_LGT_PHRASER_HEAD_H

//
// Created by DELL on 2022/4/26.
//
#include <bits/stdc++.h>
#include "Phraser_Head.h"
#include "Lexer_Head.h"
using namespace std;

set<char> VT_2;
set<char> VN_2;   // ������@
set<char> VN_FOR_TITLE; // GOTO ��ͷ
vector<char> chars_for_analyse; // ���ڴ洢Ҫ�ƽ�������Լ���ַ���
unordered_map<char,bool> VN_ifcanbe_epsilon;    // ����vn�Ƿ�����Ƴ���  GOAL 1
map<int,pair<char,string>>  PRODUCTION_FOR_ACTION;    // ����action����й�Լʱ�����кŲ��ҹ�Լ���ĸ�vn
map<char,RIGHT_SET> PRODUCTION_SOURCE;      // init������ʽ�������ʽ
map<char,set<char>> VN_FIRST;   // VN��first��
map<char,bool> VN_getfirst_ifdone;  // �����Ƿ��Ѿ������ȡfirst���Ĳ���
map<char,set<int>> line_search_for_closure;  // ͳ��ͬһvnΪ�󲿵Ĳ���ʽ�к�
set<pair<pair<int,char>,int>,compare> MOVE_OF_NEW_STATE;  // ��¼��״̬��ת��matrix
set<pair<pair<int,char>,int>,compare> GOTO_TABLE;  // GOTO��
set<pair<pair<int,char>,ACTION_item>,compare> ACTION_TABLE; // ACTION��
map<pair<int ,char>,int > goto_for_search;          // ���ڴ�ӡ�Ͳ�ѯɨ��
map<pair<int ,char>,ACTION_item > action_for_search;
unordered_map<int,PROJECT_SET> NEW_STATE;  // ��������index����״̬
int STATE_NUM=0;   // ״̬��0��ʼ����
extern map<int,pair<pair<int,string>,char>> for_task2; // �����ַ��Ų�ѯ�к�\λ��; �ַ�index��<�кţ��ַ�>

ofstream output_info_2;           //  ����ķ������ľ�����Ϣ
ofstream output_2;                //  ���yes|no �����������кź���ʾ��Ϣ

void split_2 (string line,vector<string>& words){
    int left=0,right=0;
    bool ifhasinword=false;  // �Ƿ��Ѿ����е�������
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
    VN_2.clear();   // ������@
    VN_FOR_TITLE.clear(); // GOTO ��ͷ
    chars_for_analyse.clear(); // ���ڴ洢Ҫ�ƽ�������Լ���ַ���
    VN_ifcanbe_epsilon.clear();    // ����vn�Ƿ�����Ƴ���  GOAL 1
    PRODUCTION_FOR_ACTION.clear();    // ����action����й�Լʱ�����кŲ��ҹ�Լ���ĸ�vn
    PRODUCTION_SOURCE.clear();      // init������ʽ�������ʽ
    VN_FIRST.clear();   // VN��first��
    VN_getfirst_ifdone.clear();  // �����Ƿ��Ѿ������ȡfirst���Ĳ���
    line_search_for_closure.clear();  // ͳ��ͬһvnΪ�󲿵Ĳ���ʽ�к�
    MOVE_OF_NEW_STATE.clear();  // ��¼��״̬��ת��matrix
    GOTO_TABLE.clear();  // GOTO��
    ACTION_TABLE.clear(); // ACTION��
    goto_for_search.clear();          // ���ڴ�ӡ�Ͳ�ѯɨ��
    action_for_search.clear();
    NEW_STATE.clear();  // ��������index����״̬
    STATE_NUM=0;   // ״̬��0��ʼ����

    ifstream grammar_2("../2_in_grammar.txt");
//    ifstream grammar_2("../2_in_grammar_test.txt");
    output_2.open("../2_out.txt");
    output_info_2.open("../info_about_task2.txt");
    string line;
    int line_index=0;
    vector<string> words;
    while(getline(grammar_2,line)) {
        line_index++;
        // line_search_for_closure��ʼ��
        if(line_search_for_closure.find(line[0])!=line_search_for_closure.end()){
            line_search_for_closure.find(line[0])->second.insert(line_index);
        }
        else{
            set<int > temp;
            temp.insert(line_index);
            line_search_for_closure.insert({line[0],temp});
        }
        // PRODUCTION_FOR_ACTION��ʼ��
        string rr=line.substr(3);
        PRODUCTION_FOR_ACTION.insert({line_index,{line[0],rr}});
        // VT_2��VN_2��ʼ��
        VN_2.insert(line[0]);
        for (int i = 3; i < line.length(); ++i) {
            if(line[i]<'A'||line[i]>'Z')
                VT_2.insert(line[i]);
        }
        // PRODUCTION_SOURCE
        auto pointer=PRODUCTION_SOURCE.find(line[0]);
        if(pointer!=PRODUCTION_SOURCE.end()){  // ling[0]Ϊ�󲿵��Ѿ�����
            rs temp(line);
            pointer->second.insert(temp);
        }
        else{       // ��һ��ling[0]Ϊ�󲿵Ĳ���ʽ��
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
    for(auto i:VN_2){             // ���Ƚ�����ȫfalse
        VN_getfirst_ifdone.insert({i,false});
    }
    output_info_2<<"                                   �﷨�����ķ� �����Ϣͳ��(��������)  "<<endl;
    output_info_2<<"����ʽ���� : "<<line_index<<endl;
    output_info_2<<"Vn��Ŀ : "<<VN_2.size()<<endl;
    output_info_2<<"        { ";
    for(auto i:VN_2)
        output_info_2<<i;
    output_info_2<<" }"<<endl;
    output_info_2<<"Vt��Ŀ : "<<VT_2.size()<<endl;
    output_info_2<<"        { ";
    for(auto i:VT_2)
        output_info_2<<i;
    output_info_2<<" }"<<endl;
}
void GOTO_ACTION_TABLE_show (){
    output_info_2<<"                            LR(1) GOTO-ACTION ��"<<endl;
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
//    for(auto i:character_fornow)  // �ָ��ֳ� stackĬ�ϴ�����ָ��
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
        for(auto i:a){  // ��ÿ��a�е�item ����line_index ��b�в���
            ifhave=false;
            for(auto j:b){   // ��һ��û���ҵ���Ϊ��
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
                if(i.right[j+1]==v){         // ���moveһλ
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
    int counter=0;               //  ͳ��һ���ϲ��˼������ŵ�first�������ȫvn�ɳ��գ������@
    for(auto i:characters){
        if(VN_2.count(i)&&VN_ifcanbe_epsilon.find(i)->second){  // ��vn�ҿ��Գ���
           counter++;
           for(auto j:VN_FIRST.find(i)->second){
               if(j!='@')
                   res.insert(j);
           }
        }
        else{       // ���ɳ��յ�vn||vt  ֱ��ֹͣ�˲�����������
           if(VT_2.count(i)) {
              res.insert(i);
              break;
           }
           else{    // ���ɳ��յ�vn
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
void vn_epsilon_process (){       // Ҫ��vn�����Ǵ�дӢ����ĸ
    set<char> VN_can_epsilon,VN_not_epsilon,VN_unknow; // ���������|��������հ���״̬;
    bool all_have_vt=true;
    bool if_added_to_must=false;       // �Ƿ��Ѽ���ǰ�������Լ���
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
           for(auto k:j.right_inorder){         // ���һ��vn�����в���ʽ���Ҳ��Ķ�����vn,�ز��ܲ�����
               if(j.right_for_compare[0]<'A'||j.right_for_compare[0]>'Z'){
                   counter++;
                   break;
               }     // ����Ҳ��д����ս��
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
        for(auto i:VN_unknow_temp){    // �Ե�һ��ɨ�費ȷ���ܷ��Ƴ��յ�vn  ����ȫ��Ϊvn�������Ҳ�ʽ�ӽ����ж�
            ifhave_added=false;
            for(auto j:PRODUCTION_SOURCE.find(i)->second){
                counter=0;
                for(auto k:j.right_inorder){
                    if(j.right_for_compare[0]>='A'&&j.right_for_compare[0]<='Z'&&VN_can_epsilon.find(k)!=VN_can_epsilon.end()){
                        counter++;
                    }
                }
                if(counter==j.right_inorder.size()){  // ���Ҳ�ȫ��vn�����ȫ���ܳ��գ������
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
    output_info_2<<"��Vn�ܷ��Ƴ��� : "<<endl;
    output_info_2<<"          ";
    for(auto i:VN_ifcanbe_epsilon){  // �����һ��vn
        output_info_2<<i.first<<"   ";
    }
    output_info_2<<endl;
    output_info_2<<"          ";
    for(auto i:VN_ifcanbe_epsilon){  // �����һ��vn
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
    int counter=0;      // ͳ�� ��ÿһ��vn �����в���ʽ�Ҳ� �����һ���ַ�Ϊvt|@�ĸ���
    for(auto i:VN_2){      // ��ÿ��vn
        counter=0;
        for(auto j:PRODUCTION_SOURCE.find(i)->second){
           if(j.if_a_single_epsilon){   // �����->@
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
           if(j.right_for_compare[0]<'A'||j.right_for_compare[0]>'Z'){ // ����Ҳ���һ��Ϊvt
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
        if(counter==PRODUCTION_SOURCE.find(i)->second.size()){     // ���ֻ���� @���Ҳ����ַ�Ϊvt���Ѿ�������first��;
            VN_getfirst_ifdone.find(i)->second=true;
            VN_FIRST.insert({i,first_temp.find(i)->second}); // �������յ�VN_FIRST
        }
    }
    set<char> VT_have_not_firsted;
    for(auto i:VN_getfirst_ifdone){   // �ҵ���δ�õ�first����vn
        if(!i.second)
            VT_have_not_firsted.insert(i.first);
    }
    bool if_all_vnbegin_right_done=true;   // ��һ����vn �Ƿ�������vn��ͷ���Ҳ���first���϶����Ա�˳������
    int right_epsilon_vn_num=0;      // ͳ�ƣ������ж��Ƿ�һ·�ߵ�ͷ���ǿ�


    int countert=0;
    while(!VT_have_not_firsted.empty()){
        countert++;
        if(countert==200)
            ;
        set<char> VT_have_not_firsted_temp;
        for(auto i:VT_have_not_firsted)
            VT_have_not_firsted_temp.insert(i);
        for(auto i:VT_have_not_firsted_temp){        // ��ÿ����һ��ɨ��û��ȡ��first����vn����ѭ����first
            if_all_vnbegin_right_done=true;
            for(auto j:PRODUCTION_SOURCE.find(i)->second){
                right_epsilon_vn_num=0;
                if(j.right_for_compare[0]>='A'&&j.right_for_compare[0]<='Z'){  // ���vn��ͷ
                    for(auto k:j.right_inorder){       // !!! ��������
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
                                        }  // ȥ��@
                                    }
                                }
                            }
                            else{    // �Ҳ��ߵ��ĵ�ǰvnfirst��û���ã�ֱ���Ƴ�����һ��
                                if(k!=i)  // X->Xi ������ѭ��
                                    if_all_vnbegin_right_done=false;
                                goto here;
                            }
                            if(VN_ifcanbe_epsilon.find(k)->second)  // �����ǰvn���Գ���
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
                if(right_epsilon_vn_num==j.right_inorder.size())        // �Ҳ�һ·Ϊ�գ���first�����Լ���@
                    first_temp.find(i)->second.insert('@');
            }
            here :
            if(if_all_vnbegin_right_done){  // ���в���ʽ���߹��ˣ�first�����Ѿ����ս�̬��
                VN_getfirst_ifdone.find(i)->second=true;
                if(first_temp.find(i)!=first_temp.end())
                    VN_FIRST.insert({i,first_temp.find(i)->second});
                VT_have_not_firsted.erase(i);
            }
        }
    }
    output_info_2<<"����Vn��First���� : "<<endl;
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
    int before,after=0;  // ���ڸ���һֱѭ����հ�ֱ����������
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
                if(*j=='.'&&counter+1<=i.right.size()){  // �ҵ��ұ߻����ַ��ĵ�����ڵ�item
                    if(VN_2.count(*(j+1))) {   // �� .vn���.����VN��հ���ͬʱͨ������first���ϸ���tail
                        string temp;
                        if(counter+2<=i.right.size())
                            temp=PRODUCTION_FOR_ACTION.find(i.line_index)->second.second.substr(counter);
                        char tail_char=i.tail;              // ����tailǰ��Ҫ��հ����Ӵ�����ͳ�ƺ�
                        string temp_tail(1,tail_char);
                        temp.append(temp_tail);
//                        temp=temp+temp_tail;
//                        temp=temp+i.tail;       C++ ������charֱ��תstring
                        set<char> tail_set= get_tail(temp);
                        auto pointer=line_search_for_closure.find(*(j+1));  // �ҵ�.��vn��Ӧ�����в���ʽ�к�
                        for(auto k:pointer->second){    // k Ϊ����ʽ�к�
                            project_set_ITEM aa(init_project_set_item(k));
                            for(auto m:tail_set){     // ���ÿ��.���vn���Գ��Ĳ���ʽ��ʹ�ò�ͬ��tail���ɲ�ͬ��item
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
    // $->S Ϊ��0�� �����ķ��ļ�������һϵ�б��
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
    // ������һ����Ŀ��
    PROJECT_SET pre_set;
    pre_set.insert(init_project_set_item(0));
    get_CLOSURE(pre_set);           // ȡCLOSURE
    NEW_STATE.insert({STATE_NUM++,pre_set});  // ������״̬
    stack<PROJECT_SET> s;
    s.push(pre_set);
    set<char> vt_or_vn_forconvert; // ����GOTO�Ŀ����ַ�
    while(!s.empty()){
        pre_set=s.top();
        s.pop();
        int pre= if_in_new_state(pre_set),aft;  // ���ڸ���LR��1��״̬ת������в���
        vt_or_vn_forconvert.clear();
        for(auto i:pre_set){               //  ��������.���Ҳ��ս�ͷ��ս��
            for (int j = 0; j < i.right.size(); ++j) {
                if(i.right[j]=='.'){
                    if(j+1<i.right.size()){
                        vt_or_vn_forconvert.insert(i.right[j+1]);
                        break;
                    }
                }
            }
        }
        if(vt_or_vn_forconvert.size()==0)       // �Ѿ�����̬�ˣ�������������ת��
            continue;
        else{                   // ��������GOTO
            for(auto i:vt_or_vn_forconvert){
                // ����ղ���������
                PROJECT_SET pre_set_copy;  // ����pre_set����
                for(auto j:pre_set)
                    pre_set_copy.insert(j);
                // ����GOTO������ȡ�հ�
                PROJECT_SET aft_set= GOTO_SET(pre_set_copy,i); // ת����.�ƶ����������Ŀ��Ϊaft_set
                get_CLOSURE(aft_set);                 //  ֻ�бհ�������tail�ſ��ܷ����ı�
                if(if_in_new_state(aft_set)==-1){
                    NEW_STATE.insert({STATE_NUM++,aft_set});  // ����newstate
                    s.push(aft_set);
                }
                // ��LR��1��״̬ת������в���
                aft= if_in_new_state(aft_set);
                MOVE_OF_NEW_STATE.insert({{pre,i},aft});
            }
        }
    }
    output_info_2<<"��Ŀ���淶�幹����� : "<<NEW_STATE.size()<<" ��"<<endl;
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
                    output_info_2<<"��";
                else
                    output_info_2<<k;
            }
            output_info_2<<','<<j.tail<<endl;
        }
        output_info_2<<endl;
        counter_a++;
    }
    output_info_2<<"��Ŀ��ת����ϵ ( MOVE_MATRIX ) : "<<endl;
    output_info_2<<"         "<<"I_before"<<"     Vt      "<<"I_after"<<endl;
    for(auto i:MOVE_OF_NEW_STATE)
        output_info_2<<"           I"<<i.first.first<<"         "<<i.first.second<<"         I"<<i.second<<endl;
    cout<<"��Ŀ���淶�幹����� : "<<NEW_STATE.size()<<" ��"<<endl;
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
                    cout<<"��";
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
    cout<<"��Ŀ��ת����ϵ ( MOVE_MATRIX ) : "<<endl;
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
    // �ȶ�goto�����  ״̬ת���л�ΪVn��
    for(auto i:MOVE_OF_NEW_STATE){
        if(VN_2.count(i.first.second))   // MOVE_OF_NEW_STATE��characterΪVnֱ�����GOTO��
            GOTO_TABLE.insert({i.first,i.second});
    }
    // �ٶ�action����� ״̬ת���л�ΪVt��
    //  S r acc���
    for(auto i:NEW_STATE){
        for(auto j:i.second){
            if(*j.right.rbegin()=='.'){    // r��acc���
                if(j.tail=='#'&&j.left_Vn=='$'&&*j.right.begin()=='S'){     // acc���
                    ACTION_item tempt('a',0);
                    if(!if_is_new_ACTION_item({{i.first,'#'},tempt}))
                        ACTION_TABLE.insert({{i.first,'#'},tempt});
                    continue;
                }
                if(VT_2.count(j.tail)){            // r���
                    ACTION_item tempt('r',j.line_index);
                    if(!if_is_new_ACTION_item({{i.first,j.tail},tempt}))
                        ACTION_TABLE.insert({{i.first,j.tail},tempt});
                }
            }
            else{     // S���               /////////////////////////////////!!!!error
                for (int k = 0; k < j.right.size(); ++k) {
                    if(j.right[k]=='.'){
                        if(VT_2.count(j.right[k+1])){
//                            auto point =MOVE_OF_NEW_STATE.find({i.first,j.right[k+1]});
                            for(auto mm:MOVE_OF_NEW_STATE){
                                if(mm.first.first==i.first&&mm.first.second==j.right[k+1]){  // S���
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
    // ��GOTO��ACTION�Ĳ�ѯ����г�ʼ������
    for(auto i:GOTO_TABLE){
        goto_for_search.insert({i.first,i.second});
        VN_FOR_TITLE.insert(i.first.second);
    }
    for(auto i:ACTION_TABLE)
        action_for_search.insert({i.first,i.second});
    GOTO_ACTION_TABLE_show();

    cout<<endl<<"LR(1) GOTO-ACTION ��"<<endl;
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
    bool if_accept_success=false;   // �Ƿ񱻳ɹ�����
    getline(input_string,temp);
    for (int i = 0; i < temp.length(); ++i)     // ���������ַ�������
        chars_for_analyse.push_back(temp[i]);
    chars_for_analyse.push_back('#');
    cout<<"�ƽ���Լ����������� info_about_task2.txt ( ʵʱ���� )"<<endl<<endl;
    // step 1 ��������
    stack<int> state;          // ״̬ջ
    stack<char> character;      // ����ջ
    vector<int> state_fornow;   // ��ǰ��ʹ�÷����ѯ�����
    vector<char> character_fornow;
    state.push(0);
    character.push('#');
    int step_num=0;      // �����
    int char_for_access=0;
    output_info_2<<"                            LR(1) ��Լ��������չʾ"<<endl;
    while(!if_accept_success){        // ע�������ޱ���ʱ��break
        output_info_2<<step_num++<<" ";
        if(step_num==2)
            ;
        get_stack_state_vector_version(state,state_fornow);
        get_stack_char_vector_version(character,character_fornow);
        for(auto i:character_fornow)  // �����ǰ����ջ
            output_info_2<<i;
        output_info_2<<"    ";
        for(auto i:state_fornow)  // �����ǰ״̬ջ
            output_info_2<<i<<",";
        output_info_2<<"    ";
        for(auto i:chars_for_analyse)  // ��� ������Ŵ�
            output_info_2<<i;
        if(action_for_search.find({state.top(),chars_for_analyse[0]})!=action_for_search.end()){  // ��ǰ״̬��action���п��Բ鵽
            auto point=action_for_search.find({state.top(),chars_for_analyse[0]});
            if(point->second.movement==1){  // S �ƽ�
                char_for_access++;
                state.push(point->second.number);
                character.push(chars_for_analyse[0]);
                chars_for_analyse.erase(chars_for_analyse.begin());
                output_info_2<<point->second.movement_chinese()<<endl;
            }
            if(point->second.movement==2){  // r ��Լ
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
                // chars_for_analyse  ��Լ���������Ŵ��޲�������һ��һ������ģ���ֻ�й�Լ����һ�ι�Լ�����
                output_info_2<<point->second.movement_chinese()<<"( ";
                output_info_2<<point_production->second.first<<"->"<<point_production->second.second<<" )"<<endl;
            }
            if(point->second.movement==3){  // acc ���� �����ѭ������
                if_accept_success= true;
                output_info_2<<point->second.movement_chinese()<<endl;
            }
        }
        else{  // ��ǰ���ɽ���Լ  ������wait for ������Ϣ����
            break;
        }
//        if_accept_success=true;  // debug
    }

//  while
    cout<<"�﷨�жϽ�� : "<<endl;
    if(if_accept_success)
        cout<<"            YES , �����趨�ķ�"<<endl;
    else{
        cout<<"            NO "<<endl<<"���ܵĴ���ԭ��������ʾ : "<<endl;
        auto point=for_task2.find(char_for_access+1); //ֻ���ƽ��Ų���
        cout<<"����λ��ΪԴ���� line "<<point->second.first.first<<" : "<<point->second.first.second;
        if(point->second.first.second=="return"){
            cout<<" ��������Ϊ"<<" main function return fault"<<endl;
        }
        else{
            if(point->second.second=='I')
                cout<<" ��������Ϊ"<<"KEYWORD error"<<endl;
            else if(point->second.second=='C')
                cout<<" ��������Ϊ"<<"IDENTIFIER define error"<<endl;
            else if(point->second.second=='F')
                cout<<" ��������Ϊ"<<"CONSTANT(float) define error"<<endl;
            else if(point->second.second=='O')
                cout<<" ��������Ϊ"<<"CONSTANT(scientific) define error";
            else if(point->second.second=='P')
                cout<<" ��������Ϊ"<<"CONSTANT(16) define error";
            else if(point->second.second=='E')
                cout<<" ��������Ϊ"<<"CONSTANT(complex) define error";
            else if(point->second.second=='K')
                cout<<" ��������Ϊ"<<"OPERATOR or DELIMITER error";
            else
                cout<<" δ֪����,���ֶ����";
        }
//        cout<<temp[char_for_access]<<temp[char_for_access-1]<<temp[char_for_access+1];

        // ��ʾԴ�����Ӧ�к� ����ͨ����tokenͬ�м��������������������������ȷ������λ��
    }
}
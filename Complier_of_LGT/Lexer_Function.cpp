//
// Created by DELL on 2022/4/26.
//
#include <bits/stdc++.h>
#include "Lexer_Head.h"
using namespace std;

int VT_NUM=0;  //84  �ս����������
int NFA_VN_NUM=0;  //33
int DFA_VN_NUM=0;
char start_state;
int DFA_start_state=0;
int pu=0;             // ��̬��¼epsilon�հ���a��ת�� ����
int error_number=0;
char final_bef='S',final_new;
set<char> VT;  //NFA�ս��
set<char> VN;  //NFA���ս��
set<int> DFA_VN; // DFA���ս��  ��Ч��ƣ�set��������ܲ�ͬ״̬��ͷ����Ԫ��Ҳ��ͬ
NEW_of_OLDstate O_IN_NEW;    // ������DFA���ս���ṹ���
set<char> nfa_final_state;
map<int,pair<pair<int,string>,char>> for_task2; // �����ַ��Ų�ѯ�к�\λ��; �ַ�index��<�кţ��ַ�>ԭ������
unordered_map<char,set<char>> VN_CLOSURE_SET;   // �ս���ıհ�
unordered_map<int ,set<char>> DFA_FINAL_STATE_FOR_PRINT;  // DFA����̬map ���ֺ�-ԭ���״̬ ��dfa_final_state �� O_IN_NEW����
unordered_map<int ,set<char>> DFA_NOTFINAL_STATE_FOR_PRINT;  // ͬ��
map<set<char>,int> dfa_state_search;   // �����ṩͨ��nfa״̬����Ѱ��dfa״̬�Ŀ��� ��O_IN_NEW ����
unordered_map<char,set<char>> useable_vt_for_NFA_VN;     // �����ռ�nfa�е���vn���õ�vt
unordered_map<int,bool> dfa_state_ifcircleandepsilon;   // ��ǰ״̬�Ƿ��Ѿ�������i��ת���ʹʷ��﷨����
unordered_map<int,bool> dfa_final_state;     // DFA��״̬�Ƿ�Ϊ�ս�̬ ������������������������Ϊ�涨ת����ֻ̬Ҫ��$���㣩  ��O_IN_NEW���г����Խ�����̬������״̬
int dfa_vn_counter=0;          // ����dfa״̬�������� ����ǵ�char intת��
int for_task2_num=1;
int line_index_for2=0;
NFA_movetable move_matrix_NFA;          // for auto ֻ�ܱ��������ܲ���
NFA_movetable move_matrix_NFA_final;       // �����ļ�����ʽ
SEARCH_NFA  move_matrix_NFA_MAP;        // ��finalת������
DFA_movetable move_matrix_DFA;
map<pair<int ,char>,int > move_matrix_DFA_FOR_SEARCH;  // ��move_matrix_DFA�ݻ��������������dfaѰ·���ʷ�����
const char keyword[50][20]={"break","case","char","continue","do","default","double","bits/stdc++.h","complex","map","vector",   // auto vector std/c++.h
                            "else","float","string","using","namespace","std","for","if","int","include","long","main","return","switch","typedef","void","unsigned","while","iostream"};

FILE* source_code=NULL;         //  Ҫ������Դ����
FILE* grammar_1=NULL;           //  �ʷ��������ķ�
ofstream output_info;           //  ����ʷ��ķ������ľ�����Ϣ
ofstream output;                //  token�����
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
int if_inNewSet (NSET & pre_set_copy,map<set<char>,int> dfa_state_search){      // ���������״̬�����ص�ǰ״̬������
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
    output_info<<"NFA move matrix ( NFA ״̬ת���� ) "<<endl;
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
    // for(auto ����ʧ��)
    //  ��move���Ծ�����û����Ӧ����
    nfa_move_item *temp=new nfa_move_item ;  // temp Ϊ�±���
    NSET * tempset=new NSET ;
    bool if_stateleft_Final;
    if_stateleft_Final=tempset->add_one_destination(state_right);  //  ��ת��Ϊ����NSET
    if(if_stateleft_Final){
        nfa_final_state.insert(state_left);     // �������̬����չʾ����
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
        nfa_move_item *temp=new nfa_move_item ;  // temp Ϊ�±���
        NSET * tempset=new NSET ;
        tempset->set_member.insert(*i->second.set_member.begin());  // ��������ĵ�һ���ȼ������
        while(1){
            i++;
            if(i!=move_matrix_NFA.end()){   //  ��֤��һ��״̬����move����matrix����NFA����
                aft={i->first.first,i->first.second};
                if(aft==bef){
                    tempset->set_member.insert(*i->second.set_member.begin());
                }
                if(aft!=bef){       // �����¼���
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
    for(auto i:move_matrix_NFA_final){            // ���map ��հ���ѯ�ٶȴ������
        turnto tt(i.first.first,i.first.second);
        move_matrix_NFA_MAP.insert((make_pair(tt,i.second.set_member)));
    }
    char before,after;
//    for(auto i=move_matrix_NFA_MAP.begin();i!=move_matrix_NFA_MAP.end();i++) {             // �ݽ���������useable_vt_for_NFA_VN
//        before=i->first.first;                                                          // �����Ը���move_matrix_nfa_final���������������������
//        set<char> *temp=new set<char>;
//        temp->insert(i->first.second);
//        auto bef_point=i;
//        while(1){
//            bef_point=i;    // unordered_map ����ͨ����������
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
            if(i!=move_matrix_NFA.end()){   //  ��֤��һ��״̬����move����matrix����NFA����
                after=i->first.first;
                if(after==before){
                    temp->insert(i->first.second);
                }
                if(after!=before){       // �����¼���
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
void get_closure (NSET & nfa_set,NFA_movetable &move_matrix_NFA_final){       //    �ķ����ʱ��Ӧ��ע�ⲻ������epsilon�հ���Σ��ᵼ��ѭ����Ȳ���Ԥ�⣬���ݹ���ÿ��ܻᱬջ
// v1.0
//    NSET temp(nfa_set);
//    for(auto i:nfa_set.set_member){  // nfa_set�е���״̬
//        for(auto j:move_matrix_NFA){
//            if(j.first.first==i&&j.first.second=='@'){
//                for(auto k:j.second.set_member){
//                    temp.set_member.insert(k);   // ��һ�����epsilon
//                    for(auto a:move_matrix_NFA){
//                        if(a.first.first==k&&a.first.second=='@'){
//                            for(auto b:a.second.set_member)
//                                temp.set_member.insert(b);  //  �ڶ������epsilon
//                        }
//                    }
//                }
//            }
//        }
//    }

// v2.0
//    NSET temp(nfa_set);           // �������
//    set<char> newstate;
//    for(auto i:temp.set_member){    // ��״̬���е�ÿ��Ԫ��
//        for(auto j:move_matrix_NFA_final){      // �� move_final ���в�ѯ@��ת�����������temp��ֱ�����ٲ�����״̬
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
//        for(auto i:newstate){    // ��״̬���е�ÿ��Ԫ��
//            for(auto j:move_matrix_NFA_final){      // �� move_final ���в�ѯ@��ת�����������temp��ֱ�����ٲ�����״̬
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
//v3.0 ͨ��mapʵ��        �ݹ�ʵ�֣�������ȣ��ٶȼӿ�
//    for(auto i:move_matrix_NFA_MAP){
//        turnto tt('S','@');
//        if(i.first.a=='S'&&i.first.b=='@'){
//            for(auto j:i.second)
//                cout<<j;
//            cout<<"above"<<endl;
//        }
//    }
//    for(auto i=move_matrix_NFA.begin();i!=move_matrix_NFA.end();++i){  // set������һ������Ҫ����������ֱ�Ӷ�Ԫ���е���ֵ���и��ģ�������ɾ����㣬��ƽ����ٲ���
//        i = move_matrix_NFA.erase(i);
//    }
//    for(auto & i:nfa_set.set_member)
//        i.insert();
    set<char> newstate;
    for(auto i:nfa_set.set_member){
        auto point=VN_CLOSURE_SET.find(i);      // �ȹ�����ȣ��ұ�״̬���п��Գ��յ�״̬
        if(point!=VN_CLOSURE_SET.end()){
            for(auto it:point->second)
                newstate.insert(it);
        }
        else
            get_char_epsilion(i,newstate);  // �ٶԸ������Գ��յ�״̬����������ȣ��ݹ�����
    }
    for(auto i:newstate)
        nfa_set.set_member.insert(i);
}
void add_NSET_toState (NSET &aft_set,NEW_of_OLDstate & O_IN_NEW,map<set<char>,int> &dfa_state_search){
    pair<int,set<char>> addin_temp(dfa_vn_counter,aft_set.set_member);
    DFA_VN.insert(dfa_vn_counter);          //  ͬʱ����̬����ȫ�� DFA ״̬����
    if(aft_set.set_member.find('$')!=aft_set.set_member.end()) //  ���ȷ�������״̬����̬
       dfa_final_state.insert({dfa_vn_counter,1});  //  ���ս�̬��
    else
        dfa_final_state.insert({dfa_vn_counter,0});     // ���ս�̬
    O_IN_NEW.insert(addin_temp);
    dfa_state_search.insert({aft_set.set_member,dfa_vn_counter});   // dfa_state_search��O_IN_NEWͬ������
    dfa_vn_counter++;
}
void initialization (){
    VT_NUM=0;  //84  �ս����������
    NFA_VN_NUM=0;  //33
    DFA_VN_NUM=0;
    DFA_start_state=0;
    pu=0;             // ��̬��¼epsilon�հ���a��ת�� ����
    error_number=0;
    final_bef='S',final_new='\0';
    VT.clear();  //NFA�ս��
    VN.clear();  //NFA���ս��
    DFA_VN.clear(); // DFA���ս��  ��Ч��ƣ�set��������ܲ�ͬ״̬��ͷ����Ԫ��Ҳ��ͬ
    O_IN_NEW.clear();    // ������DFA���ս���ṹ���
    nfa_final_state.clear();
    for_task2.clear(); // �����ַ��Ų�ѯ�к�\λ��; �ַ�index��<�кţ��ַ�>ԭ������
    VN_CLOSURE_SET.clear();   // �ս���ıհ�
    DFA_FINAL_STATE_FOR_PRINT.clear();  // DFA����̬map ���ֺ�-ԭ���״̬ ��dfa_final_state �� O_IN_NEW����
    DFA_NOTFINAL_STATE_FOR_PRINT.clear();  // ͬ��
    dfa_state_search.clear();   // �����ṩͨ��nfa״̬����Ѱ��dfa״̬�Ŀ��� ��O_IN_NEW ����
    useable_vt_for_NFA_VN.clear();     // �����ռ�nfa�е���vn���õ�vt
    dfa_state_ifcircleandepsilon.clear();   // ��ǰ״̬�Ƿ��Ѿ�������i��ת���ʹʷ��﷨����
    dfa_final_state.clear();     // DFA��״̬�Ƿ�Ϊ�ս�̬ ������������������������Ϊ�涨ת����ֻ̬Ҫ��$���㣩  ��O_IN_NEW���г����Խ�����̬������״̬
    dfa_vn_counter=0;          // ����dfa״̬�������� ����ǵ�char intת��
    for_task2_num=1;
    line_index_for2=0;
    move_matrix_NFA.clear();          // for auto ֻ�ܱ��������ܲ���
    move_matrix_NFA_final.clear();       // �����ļ�����ʽ
    move_matrix_NFA_MAP.clear();        // ��finalת������
    move_matrix_DFA.clear();
    move_matrix_DFA_FOR_SEARCH.clear();  // ��move_matrix_DFA�ݻ��������������dfaѰ·���ʷ�����

    source_code= fopen("../1_in_code.txt","r+");  // ��Ҫ������Դ�����ļ�������scanner�������дʷ�����
    grammar_1=fopen("../1_in_grammar.txt","r+");              // ���ķ��� LexicalAnalysis_lhk
//    grammar_1=fopen("../1_test.txt","r+");
    output_string.open("../2_in_string.txt");
    output.open("../2_in_token.txt");               // ���token��
    output_info.open("../info_about_task1.txt");    // ����ķ�ת����tokenʶ�������ϸ��Ϣ
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
bool if_dfa (bool iffirstxiaoyu,bool iffirstdayu,int line_index,string word,DFA_movetable move_matrix_DFA,map<pair<int ,char>,int> DFA_SEARCH) {
    int pre=0;   // ת����dfa״̬
    // ����Ѿ�����һ����̬���򲻿��ٽ��б����(���ǽ����п��Ա�����ǵ�ǰ״̬)
    final_bef='S',final_new='\0';
    map<pair<int ,char>,int>::iterator pointer;
    unordered_map<int,bool>::iterator if_in_final;   // Ŀ�꣺����¼������̬
    set<char> bef_final,new_final,add_final;   // �ϸ�״̬��final ��ǰ��final ������final
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
                    cout<<word<<" ����(8����) ������� because of "<<word[j]<<endl;
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
                    cout<<word<<" ����(��ѧ������) ������� because of "<<word[j]<<endl;
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
//        // ������һ�αհ�ת��
//        pointer=DFA_SEARCH.find({pointer->second,'@'});
        if(dfa_final_state.find(pre)->second){    // �����ǰdfa״̬Ϊ��̬ ,��bef����final ����
            for(auto ok:DFA_FINAL_STATE_FOR_PRINT.find(pre)->second){
                for(auto kk:nfa_final_state ){
                    if(kk==ok)
                        bef_final.insert(kk);
                }
            }
            if(bef_final.size()==1)  // 1.25E+4
                final_bef=*bef_final.begin();
        }
        if(pointer!=DFA_SEARCH.end()){  // �ɽ��� dfa move
           pre=pointer->second;          // ʹ���ַ���ǰ״̬��
           if_in_final=dfa_final_state.find(pre);
           if(if_in_final->second){  // �����ǰ�Ѿ���dfa�ս�״̬���ҳ���Ӧnfa״̬�е��ս�״̬
               for(auto jj:DFA_FINAL_STATE_FOR_PRINT.find(pre)->second){  // �õ� new����final
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
                       else{   // ���ִ���
                           if(final_bef=='I'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" ��ʶ�� ������� because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='C'&&final_new=='I'){   //  ���� 000aaa
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" ��ʶ�� ������� because of "<<word[i-1]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='C'){
                               error_number++;
                               cout<<error_number<<". ";
//                               cout<<final_bef<<final_new;
                               cout<<word<<" ���� ������� because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='F'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" ����(������) ������� because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='O'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" ����(��ѧ������) ������� because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='D'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" ����(16������) ������� because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='E'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" ����(����) ������� because of "<<word[i]<<endl;
                               output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                               output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                               return false;
                           }
                           if(final_bef=='K'){
                               error_number++;
                               cout<<error_number<<". ";
                               cout<<word<<" �����||�޶��� ������� because of "<<word[i]<<endl;
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
//               if(add_final.size()>=2){             // ��֤�ķ�ÿһ������״̬���ܳ���һ�� ���Կ���
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
                cout<<word<<" ��ʶ�� ������� because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='C'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" ���� ������� because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='F'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" ����(������) ������� because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='O'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" ����(��ѧ������) ������� because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='P'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" ����(16������) ������� because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='E'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" ����(����) ������� because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            if(final_bef=='K'){
                error_number++;
                cout<<error_number<<". ";
                cout<<word<<" �����||�޶��� ������� because of "<<word[i]<<endl;
                output<<setiosflags(ios::left)<<setw(16)<<"ERROR TYPE";
                output<<"   "<<setiosflags(ios::left)<<setw(16)<<word<<endl;
                return false;
            }
            error_number++;
            cout<<error_number<<". ";
            cout<<word<<" ������� because of "<<word[i]<<endl;
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
    char line[100];             //   ʹ��fgets�����ȷ����ڴ棬���ɿ�ָ��
//    line.resize(7);  // �����ڴ�������fscanf ��������resize(��capacity)
    char state_left,state_right;
    char vt;
    fseek(grammar_1,0,SEEK_SET);
    fgets(line,5,grammar_1);
    string numofline(line);
    n= stoi(numofline);
    output_info<<"                                   �ʷ������ķ� �����Ϣͳ��(��������)  "<<endl;
    output_info<<"����ʽ���� : "<<n<<endl;
    _for(i,0,n){                            // less ABKY IY
//        fscanf(grammar_1,"%s",&line[0]);
        fgets(line,100,grammar_1);
        state_left=line[0];
        vt=line[3];
        if(i==0){               // ��̬����
            start_state=state_left;
        }
        if(!if_inVN(state_left,VN))  // ���󲿽������
            VN.insert(state_left);
        if(!if_inVT(vt,VT))     // ���Ҳ����ս���������
            VT.insert(vt);
        if(((int)line[4]>=(int)'A'&&(int)line[4]<=(int)'Z')||((int)line[4]>=(int)'a'&&(int)line[4]<=(int)'f')){                // �Ҳ���VN ��move������ӱ���
            state_right=line[4];
        }
        else{                       // ��VN ����߲������ (vt)->#(��̬)
            state_right='#';
        }
        add_nfaToMovetable(state_left,vt,state_right,move_matrix_NFA); //  ���nfa move����
    }
    get_NFA_movetable_final(move_matrix_NFA);
    NFA_VN_NUM=VN.size();
    VT_NUM=VT.size();
    output_info<<"Vn��Ŀ : "<<NFA_VN_NUM<<endl;
    output_info<<"        { ";
    for(auto i:VN)
        output_info<<i;
    output_info<<" }"<<endl;
    output_info<<"Vt��Ŀ : "<<VT_NUM<<endl;
    output_info<<"        { ";
    for(auto i:VT)
        output_info<<i;
    output_info<<" }"<<endl;
    output_info<<"��̬ : "<<start_state<<endl;
    output_info<<"NFA��̬��Ŀ : "<<nfa_final_state.size()<<endl;
    output_info<<"        { ";
    for(auto i:nfa_final_state)
        output_info<<i;
    output_info<<" }"<<endl;
    show_NFA_movetable(move_matrix_NFA_final);
}
void NFA_to_DFA (){
    NSET pre_set;
    pre_set.set_member.insert(start_state);     // �ȴ����̬
    stack<NSET> s;
    get_closure(pre_set,move_matrix_NFA);  // !!!!!!!wait for write  ������->@ �󼯺�����$
    s.push(pre_set);
//    for(auto i:pre_set.set_member)
//        cout<<i;
//    cout<<endl;
    DFA_start_state=0;   // 0״̬����ʼ״̬
    add_NSET_toState(pre_set,O_IN_NEW,dfa_state_search); // �õ���״̬��ɵ���״̬�ṹ�壬�����������     (��NSET���NEW_of_OLDstate�е�Ԫ��)
    pu=1;  // ��ǰ�еڼ���
    set<char> temp_allA_inSET;     // ״̬����������״̬���Խ��е�a����(��֦������Ż�����ʱ�䣬�Զ�״̬����Ч������)
    map<set<char>,int> cut_tree;   //  ͬһ�е�������״̬��ͬ�������ٶ���ȡ�հ������״̬�ˣ�ֱ���˳�����
    set<char> add_state;        // ��ǰ��״̬����i��ת����������״̬
    map<char,char> for_quickcompare;
    map<int,bool> have_gothrough_line_states;  // һ��״̬�Ƿ��Ѿ��߹�������һ����
    have_gothrough_line_states.clear();

    int counter=0;

    while(!s.empty()){

        pre_set=s.top();
        s.pop();
        int bef= if_inNewSet(pre_set,dfa_state_search); // һ�е���ʼ״̬����
        if(have_gothrough_line_states.find(bef)!=have_gothrough_line_states.end())
            continue;
        pu=1;

        for_quickcompare.clear();
        for(auto i:pre_set.set_member)    // ��������״̬�ⷨ�Ż�
            for_quickcompare.insert({i,i});

        temp_allA_inSET.clear();
        for (auto i:pre_set.set_member) {   // ��pre_set״̬������ÿ��״̬������a��ת����ȡ�հ�
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
//  ��ȡ�ÿ���ʹ�õ�a������
        // ���Ѿ���С�Ŀ��ܷ���״̬ת�Ƶ�a�����ϣ���ѯpre_set����״̬�����Ƿ���ת��������������Ӽ�

        cut_tree.clear();

        for (auto i:temp_allA_inSET) {          // ���� i ��ת��
            add_state.clear();
            NSET pre_set_copy(pre_set);     // !!!!ע��ÿ��һ�αհ��Լ�ת��֮ǰ��pre_set���ƿ���
            for (auto j:pre_set.set_member) {   // j ״̬����״̬����
                pair<char,char> match_move(j,i);    // j״̬����i��ת��
                auto mm=move_matrix_NFA_MAP.find(match_move);   //  ���ҵ�ת��������״̬
                if(mm!=move_matrix_NFA_MAP.end()){
                    pre_set_copy.set_member.insert(mm->second.begin(),mm->second.end());
//                    add_state.insert(mm->second.begin(),mm->second.end());
                    for(auto kkl:mm->second){    // ������ȷ������״̬��
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
            if(temppointer!=cut_tree.end()&&pu!=1) {  //  �Ե�ǰ״̬����հ������ѹ��״̬�Ƕ����
                goto here1;        // goto ֮�󲻿�����������
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
            if(num==-1){             //  ��ǰ״̬Ϊ��״̬
//                for(auto jjo:pre_set_copy.set_member){
//                    if(jjo=='$')    // �Ѿ����ս�̬�ˣ������ٽ���push���бհ�ת����
//                      goto jjj;
//                }
                s.push(pre_set_copy);
            jjj:
                add_NSET_toState(pre_set_copy,O_IN_NEW,dfa_state_search);
                cut_tree.insert({add_state,dfa_vn_counter-1});
            }
            aft= if_inNewSet(pre_set_copy,dfa_state_search);
            tempformoveitem={{bef,i},aft};
            move_matrix_DFA.insert(tempformoveitem); //�����ǲ�����״̬��Ҫ��dfa����move�������
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
    //  �����Ϣ��output_info
    output_info<<""<<endl;
    output_info<<"                                   NFA -> DFA Convert ( NFA ȷ������� )  "<<endl;
    output_info<<"                     ���� "<<pu<<" ����Ч��epsilon�հ���a��ת��,�� "<<NFA_VN_NUM<<" ��NFA״̬��ת��Ϊ "<<DFA_VN_NUM<<" ��DFA״̬"<<endl;
    output_info<<"Vn��Ŀ : "<<DFA_VN_NUM<<" ( ״̬�����ֱ�ʾ )"<<endl;
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
    output_info<<"Vt��Ŀ : "<<VT_NUM<<endl;
    output_info<<"        { ";
    for(auto i:VT)
        output_info<<i;
    output_info<<" }"<<endl;
    output_info<<"��̬ : "<<DFA_start_state<<" { ";
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
    output_info<<"DFA��̬��Ŀ : "<<DFA_FINAL_STATE_FOR_PRINT.size()<<" �� �� ״̬ { ��ɸ�DFA״̬��NFA״̬���� } �� ����ʽ������� :"<<endl;
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
    output_info<<"                                   DFA ״̬ת�ƾ���   "<<endl;
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
    map<pair<int ,char>,int> DFA_SEARCH;  // ����dfa��ѯ
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
    cout<<"Դ����������������ʾ :"<<endl;

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
        cout<<endl<<"SUCCESS----�ļ���Դ��������ķ��淶";
}
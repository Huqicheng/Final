#include "helper.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include "command.hpp"
#include "graph.hpp"

using namespace std;
using namespace stringutils;

pthread_mutex_t mut_output = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_input = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_input_flag = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_cnt = PTHREAD_MUTEX_INITIALIZER;

int cnt = 0;
int input_flag = 1;

/* global data structure*/
Graph g;
vector<int> resultVC4a1;
vector<int> resultVC4a2;
vector<int> resultVC4a3;

vector<vector<int>> edgeCache;

void reset(){
    g.reset();
    resultVC4a1.clear();
    resultVC4a2.clear();
    resultVC4a3.clear();
    edgeCache.clear();
}



void printResult(string name,vector<int>& result){
    cout << name << ":";
    for(unsigned int i=0;i<result.size();i++){
        if(i != result.size()-1)
            cout<<result[i]<<",";
        else
            cout<<result[i]<<endl;
    }
}

/* to simulate 3 algorithms :(*/

//a1 is to mutiply initial by 1
void* a1(void* args){
    
    mulock(LOCK,&mut_input);
    
    //algorithm
    vector<vector<int>> edges;
    edges = edgeCache;
    
    
    set<int> setVertices;
    
    vector<vector<int>>::iterator edgeIterator = edges.begin();
    
    while(edgeIterator!=edges.end()){
        vector<int> edge = *edgeIterator;
        setVertices.insert(edge[0]);
        setVertices.insert(edge[1]);
        vector<vector<int>>::iterator ite = edgeIterator+1;
        for(;ite!=edges.end();ite++){
            vector<int> vect = *ite;
            if(vect[0] == edge[0] || vect[0] == edge[1] || vect[1] == edge[0] || vect[1] == edge[1]){
                edges.erase(ite);
                ite--;
            }
        }
        
        
        edges.erase(edgeIterator);
    }
    
    set<int>::iterator iteSet;
    for(iteSet = setVertices.begin();iteSet!=setVertices.end();iteSet++){
        resultVC4a1.push_back(*iteSet);
    }
    
    
    mulock(LOCK,&mut_cnt);
    cnt++;
    if(cnt == 3){
        mulock(UNLOCK,&mut_output);
    }
    mulock(UNLOCK,&mut_cnt);
    
    mulock(UNLOCK,&mut_input);
}

//a2 is to mutiply initial by 2
void* a2(void* args){
    
    mulock(LOCK,&mut_input);
    
    Graph tmpG = g;
    set<vector<int>> edges;
    set<int> setVertices;
    cout << "landmark0"<< endl;
    tmpG.getEdges(edges);
    cout << "landmark1"<< endl;
    while(!edges.empty()){
        
        int idx = 0;
        cout << "idx:" << idx << endl;
        idx = tmpG.getIdxOfVertexWithMaxDegree(setVertices);
        cout << "idx:" << idx << endl;
        set<vector<int>>::iterator ite;
        setVertices.insert(idx);
        cout << "idx:" << idx << endl;
        for(ite=edges.begin();ite!=edges.end();){
            vector<int> edge = *ite;
            if(edge[0] == idx || edge[1] == idx){
                tmpG.removeEdge(edge[0],edge[1]);
                ite = edges.erase(ite);
            }else{
                ite++;
            }
            
        }
  
        
    }
    set<int>::iterator iteInt;
    for(iteInt = setVertices.begin();iteInt!=setVertices.end();iteInt++){
        resultVC4a2.push_back(*iteInt);
    }
    
    
    mulock(LOCK,&mut_cnt);
    cnt++;
    if(cnt == 3){
        mulock(UNLOCK,&mut_output);
    }
    mulock(UNLOCK,&mut_cnt);
    
    mulock(UNLOCK,&mut_input);
}

//a3 is to mutiply initial by 3
void* a3(void* args){
    
    mulock(LOCK,&mut_input);
    
    //algorithm
    
    
    mulock(LOCK,&mut_cnt);
    cnt++;
    if(cnt == 3){
        mulock(UNLOCK,&mut_output);
    }
    mulock(UNLOCK,&mut_cnt);
    mulock(UNLOCK,&mut_input);
}

void* io(void* args){
    
    //start your input here
    //V
    if(!cin.eof()){
        string str;
        
        std::getline(std::cin, str);
        if(str == "") {
            input_flag = 0;
            return NULL;
        }
        //cout << "input:" << str << endl;
        int opType = -1;
        int num = 0;
        string errMsg = "";
        vector<vector<int> > vect;
        if(!parseLine(str,opType,num,vect,errMsg))
        {
            cerr<<"Error: "<<errMsg<<endl;
            input_flag = 0;
            return NULL;
        }
        if(opType != VERTEX){
            cerr<<"Error: "<<"command is not in order"<<endl;
            input_flag = 0;
            return NULL;

        }
        g.setVertexNum(num);
        g.reset();
        
    }else{
        input_flag = 0;
        return NULL;
    }
    
    //E
    if(!cin.eof()){
        string str;
        
        std::getline(std::cin, str);
        if(str == "") {
            input_flag = 0;
            return NULL;
        }
        //cout << "input:" << str << endl;
        int opType = -1;
        int num = 0;
        string errMsg = "";
        vector<vector<int> > vect;
        if(!parseLine(str,opType,num,vect,errMsg))
        {
            cerr<<"Error: "<<errMsg<<endl;
            input_flag = 0;
            return NULL;
        }
        if(opType != EDGE){
            cerr<<"Error: "<<"command is not in order"<<endl;
            input_flag = 0;
            return NULL;
            
        }
        for(unsigned int i=0; i<vect.size(); i++)
        {
            if(!g.addAnEdge(vect[i][0],vect[i][1],errMsg))
            {
                cerr <<"Error: "<<errMsg <<endl;
                g.reset();
                input_flag = 0;
                return NULL;
            }
        }
        
        
        edgeCache = vect;
        
    }else{
        input_flag = 0;
        return NULL;
    }


    
    
    mulock(UNLOCK,&mut_input);
    mulock(LOCK,&mut_output);
    
    // start your output here
    //g.printGraph();
    printResult("a1",resultVC4a1);
    printResult("a2",resultVC4a2);
    
    
    mulock(LOCK,&mut_cnt);
    //cout << "set counter 0" << endl;
    cnt = 0;
    mulock(UNLOCK,&mut_cnt);
    
    reset();
    
    
    mulock(UNLOCK,&mut_output);
    
    
    
    
    
    
}

int main(){
    
    while(true){
    
    // ensure that mut_input and mut_output will not be locked until the io thread terminates

        
    mulock(LOCK,&mut_input);
    mulock(LOCK,&mut_output);
    
    pthread_t thread_a1,thread_a2,thread_a3,thread_io;
    void* res_a1,*res_a2,*res_a3;
    
    
    // 3 algorithms run concurrently
    if (pthread_create(&thread_a1, NULL, &a1, (void *)NULL) == -1) {
        puts("fail to create pthread thread_a1");
        exit(1);
    }

    
    if (pthread_create(&thread_a2, NULL, &a2, (void *)NULL) == -1) {
        puts("fail to create pthread thread_a2");
        exit(1);
    }

    
    if (pthread_create(&thread_a3, NULL, &a3, (void *)NULL) == -1){
        puts("fail to create pthread thread_a3");
        exit(1);
    }
        

    
    if (pthread_create(&thread_io, NULL, &io, (void *)NULL) == -1){
        puts("fail to create pthread thread_a3");
        exit(1);
    }
    
    // wait for io to terminate
    if (pthread_join(thread_io,&res_a3) == -1){
        puts("fail to recollect thread_io");
        exit(1);
    }
        
    // if io terminated, check if it's eof
    if(!input_flag){
        //jump out of while loop
        break;
        
    }
        
    }
    
            

}

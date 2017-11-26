#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include "minisat/mtl/Vec.h"
#include "helper.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include "command.hpp"
#include "graph.hpp"

using namespace std;
using namespace stringutils;

// encapsulate Minisat::Solver in SatSolver to adapt the graph structure
class SatSolver{
    private:
    Minisat::Solver* solver;
    vector<vector<Minisat::Lit> > vectLits;
    int row,col;
    public:
    SatSolver(){
        solver = new Minisat::Solver();
    }
    void setRow(int r){row = r;}
    void setCol(int c){col = c;}
    int getRow(){return row;}
    int getCol(){return col;}
    void addClause(vector<vector<int> > idx, vector<int> sign);
    void addLiteral();
    Minisat::lbool solve();
    void resetSolver();
    void getModel(vector<vector<int> >& res);
    ~SatSolver(){
        delete solver;
    }
};

void SatSolver::addClause(vector<vector<int> > idx, vector<int> sign){
    Minisat::vec<Minisat::Lit> vect;
    for(unsigned int i=0; i<idx.size(); ++i){
        
        vector<int> index = idx[i];
        int s = sign[i];
        if(s == 0){
            vect.push(~vectLits[index[0]][index[1]]);
        }else if(s == 1){
            vect.push(vectLits[index[0]][index[1]]);
        }
    }
    
    solver->addClause(vect);
}

void SatSolver::addLiteral(){
    for(int i=0; i<row; ++i){
        vector<Minisat::Lit> tmp;
        for(int j=0;j<col;++j){
            tmp.push_back(Minisat::mkLit(solver->newVar()));
        }
        vectLits.push_back(tmp);
    }
}

Minisat::lbool SatSolver::solve(){
    if(!solver->simplify()){
        cout<<col<<" cannot be simplified" << endl;
        return Minisat::l_False;
    }
    Minisat::vec<Minisat::Lit> dummy;
    return solver->solveLimited(dummy);
}

void SatSolver::resetSolver(){
    //reset solver
    delete solver;
    solver = new Minisat::Solver();
    //reset lits
    vectLits.clear();
    
}

// get the assignment which satisfies CNF
void SatSolver::getModel(vector<vector<int> >& res){
    for(int i=0;i<row;++i){
        vector<int> tmp;
        for(int j=0;j<col;++j){
            tmp.push_back(Minisat::toInt(solver->modelValue(vectLits[i][j])));
        }
        res.push_back(tmp);
    }
}


pthread_mutex_t mut_output = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_input = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_input_flag = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_cnt = PTHREAD_MUTEX_INITIALIZER;

int cnt = 0;
int input_flag = 1;

/* global data structure*/
Graph g;
vector<int> resultVC4Approx1;
vector<int> resultVC4Approx2;
vector<int> resultVC4CNF;

vector<vector<int>> edgeCache;

void reset(){
    g.reset();
    resultVC4Approx1.clear();
    resultVC4Approx2.clear();
    resultVC4CNF.clear();
    edgeCache.clear();
}



void printResult(string name,vector<int>& result){
    cout << name << ": ";
    for(unsigned int i=0;i<result.size();i++){
        if(i != result.size()-1)
            cout<<result[i]<<",";
        else
            cout<<result[i];
    }
    cout<<endl;
}




// to check if k is a correct size of vertex cover of the graph
Minisat::lbool solve(Graph& g , int k, vector<vector<int> >& res){
    
    // initialize the solver
    SatSolver solver;
    solver.setRow(g.getVertexNum());
    solver.setCol(k);
    solver.addLiteral();
    
    //encoding 1
    for(int i=0;i<k;++i){
        vector<vector<int> > idx;
        vector<int> sign;
        for(int j=0;j<g.getVertexNum();++j){
            vector<int> index;
            index.push_back(j);
            index.push_back(i);
            idx.push_back(index);
            sign.push_back(1);
        }
        solver.addClause(idx,sign);
    }
    
    //encoding 2
    for(int i=0;i<g.getVertexNum();++i){
        for(int p=0;p<k;++p){
            for(int q=p+1;q<k;++q){
                vector<vector<int> > idx;
                vector<int> sign;
                
                vector<int> index;
                index.push_back(i);
                index.push_back(p);
                idx.push_back(index);
                sign.push_back(0);
                
                vector<int> index2;
                index2.push_back(i);
                index2.push_back(q);
                idx.push_back(index2);
                sign.push_back(0);
                
                solver.addClause(idx,sign);
            }
        }
    }
    
    //encoding 3
    for(int i=0;i<k;++i){
        for(int p=0;p<g.getVertexNum();++p){
            for(int q=p+1;q<g.getVertexNum();++q){
                vector<vector<int> > idx;
                vector<int> sign;
                
                vector<int> index;
                index.push_back(p);
                index.push_back(i);
                idx.push_back(index);
                sign.push_back(0);
                
                vector<int> index2;
                index2.push_back(q);
                index2.push_back(i);
                idx.push_back(index2);
                sign.push_back(0);
                
                solver.addClause(idx,sign);
            }
        }
    }
    
    //encoding 4
    vector<vector<int> > edges;
    g.getEdges(edges);
    for(unsigned int a=0;a<edges.size();++a){
        int i = edges[a][0];
        int j = edges[a][1];
        vector<vector<int> > idx;
        vector<int> sign;
        for(int s = 0;s<k;++s){
            vector<int> index;
            index.push_back(i);
            index.push_back(s);
            idx.push_back(index);
            sign.push_back(1);
            
            vector<int> index2;
            index2.push_back(j);
            index2.push_back(s);
            idx.push_back(index2);
            sign.push_back(1);
        }
        solver.addClause(idx,sign);
    }
    //solve SAT
    
    Minisat::lbool tag = solver.solve();
    if(tag == Minisat::l_True){
        solver.getModel(res);
    }
    return tag;
}


/* to simulate 3 algorithms :(*/

//approx 2
void* APPROX_VC_2(void* args){
    
    mulock(LOCK,&mut_input);
    if(edgeCache.size() == 0){
        // do nothing
    }else{
        
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
            resultVC4Approx2.push_back(*iteSet);
        }
        
    }
    
    clockid_t cid;
    int retcode;
    retcode = pthread_getcpuclockid(pthread_self(), &cid);
    
    if(retcode) {
        errp("a1 - pthread_getcpuclockid", retcode);
    }
    else {
        pclock("a1 - Thread cpu time: ", cid);
    }
    
    
    mulock(LOCK,&mut_cnt);
    cnt++;
    if(cnt == 3){
        mulock(UNLOCK,&mut_output);
    }
    mulock(UNLOCK,&mut_cnt);
    mulock(UNLOCK,&mut_input);
}

//approx 1
void* APPROX_VC_1(void* args){
    mulock(LOCK,&mut_input);
    if(edgeCache.size() == 0){
        // do nothing
    }else{
        Graph tmpG = g;
        set<vector<int>> edges;
        set<int> setVertices;
        tmpG.getEdges(edges);
        while(!edges.empty()){
            int idx = 0;
            idx = tmpG.getIdxOfVertexWithMaxDegree(setVertices);
            set<vector<int>>::iterator ite;
            setVertices.insert(idx);
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
            resultVC4Approx1.push_back(*iteInt);
        }

    }
    
    clockid_t cid;
    int retcode;
    retcode = pthread_getcpuclockid(pthread_self(), &cid);
    
    if(retcode) {
        errp("a2 - pthread_getcpuclockid", retcode);
    }
    else {
        pclock("a2 - Thread cpu time: ", cid);
    }
    

    
    mulock(LOCK,&mut_cnt);
    cnt++;
    if(cnt == 3){
        mulock(UNLOCK,&mut_output);
    }
    mulock(UNLOCK,&mut_cnt);
    
    mulock(UNLOCK,&mut_input);
}

//sat
void* CNF_SAT_VC(void* args){
    
    mulock(LOCK,&mut_input);
    
    if(edgeCache.size() == 0){
        
    }else{
        for(int k=1; k<g.getVertexNum()-1;k++){
            vector<vector<int> > res;
            Minisat::lbool tag = solve(g,k,res);
            if(tag == Minisat::l_True){
                for(unsigned int ii = 0;ii<res.size();++ii){
                    for(unsigned int jj=0;jj<res[ii].size();++jj){
                        if(res[ii][jj] == 0){
                            resultVC4CNF.push_back(ii);
                        }
                    }
                }
                break;
            }
        }
    }
    
    clockid_t cid;
    int retcode;
    retcode = pthread_getcpuclockid(pthread_self(), &cid);
    
    if(retcode) {
        errp("a3 - pthread_getcpuclockid", retcode);
    }
    else {
        pclock("a3 - Thread cpu time: ", cid);
    }
    
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
    if(cin.peek() != EOF){
        string str;
        
        std::getline(std::cin, str);
//        if(str == "") {
//            input_flag = 0;
//            return NULL;
//        }
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
    if(cin.peek() != EOF){
        string str;
        
        std::getline(std::cin, str);
//        if(str == "") {
//            input_flag = 0;
//            return NULL;
//        }
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
    g.printGraph();
    printResult("CNF-SAT-VC",resultVC4CNF);
    printResult("APPROX-VC-1",resultVC4Approx1);
    printResult("APPROX-VC-2",resultVC4Approx2);
    
    
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
        if (pthread_create(&thread_a1, NULL, &APPROX_VC_1, (void *)NULL) == -1) {
            puts("fail to create pthread thread_a1");
            exit(1);
        }
    
        if (pthread_create(&thread_a2, NULL, &APPROX_VC_2, (void *)NULL) == -1) {
            puts("fail to create pthread thread_a2");
            exit(1);
        }
    
        if (pthread_create(&thread_a3, NULL, &CNF_SAT_VC, (void *)NULL) == -1){
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
            int ret;
            //kill threads
            if(ret = pthread_kill(thread_a1, 0)) {
                // already terminated
            }
            else {
                //cancel thread if it's alive
                ret = pthread_cancel(thread_a1);
            }
            
            if(ret = pthread_kill(thread_a2, 0)) {
                // already terminated
            }
            else {
                //cancel thread if it's alive
                ret = pthread_cancel(thread_a2);
            }
            
            if(ret = pthread_kill(thread_a3, 0)) {
                // already terminated
            }
            else {
                //cancel thread if it's alive
                ret = pthread_cancel(thread_a3);
            }
            
            //jump out of while loop
            break;
        }
        
    }
    
            

}

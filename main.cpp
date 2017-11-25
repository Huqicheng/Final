#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include "minisat/mtl/Vec.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include "graph.hpp"
#include "command.hpp"
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




void* solveBySat(Graph& g){
    int left=1, right=g.getVertexNum()-1;
    int resK = -1;
    vector<int> resV;
    while(left<=right){
        int k = (right-left)/2+left;
        
        vector<vector<int> > res;
        Minisat::lbool tag = solve(g,k,res);
        
        // if tag == true, then current k is guaranteed to be the minimum
        if(tag == Minisat::l_True){
            vector<int> vertices;
            // vector vertices is guaranteed to be in ascending order
            for(unsigned int ii = 0;ii<res.size();++ii){
                for(unsigned int jj=0;jj<res[ii].size();++jj){
                    if(res[ii][jj] == 0){
                        vertices.push_back(ii);
                    }
                }
            }
            
            resK = k;
            resV = vertices;
            
            right = k-1;
            
        }else{
            left = k+1;
            
        }
        
    }
    for(unsigned int ii=0;ii<resV.size();++ii){
        if(ii == resV.size()-1){
            cout<<resV[ii]<<endl;
        }else{
            cout<<resV[ii]<<" ";
        }
        
    }
    
}

void* solveByApprox1(Graph& g){
    Graph tmpG = g;
    set<vector<int>> edges;
    set<int> setVertices;
    
    tmpG.getEdges(edges);
    while(!edges.empty()){
        
        int idx = tmpG.getIdxOfVertexWithMaxDegree(setVertices);
        
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
//            cout << "edge is empty" << edges.empty() << endl;
//            cout << "ite is end" << (edges.end() == ite) << endl;
        }
        
//        cout << "edges is empty:" << edges.empty() << endl;
        
    }
    set<int>::iterator iteInt;
    for(iteInt = setVertices.begin();iteInt!=setVertices.end();iteInt++){
        cout << *iteInt << endl;
    }
    
    
    
}

void* solveByApprox2(vector<vector<int>> edges){
    
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
//        if(iteSet != setVertices.end()-1)
//            cout<<*iteSet<<" ";
//        else
//            cout <<*iteSet <<endl;
    }
    
    
    
    
}
int main(int argc, char** argv)
{
    Graph g;
    
    while (!std::cin.eof())
    {
        std::string cmd;
        std::getline(std::cin, cmd);
        if(cmd == "") continue;
        int opType = -1;
        int num = 0;
        string errMsg = "";
        vector<vector<int> > vect;
        if(!parseLine(cmd,opType,num,vect,errMsg))
        {
            cerr<<"Error: "<<errMsg<<endl;
            continue;
        }

        if(opType == VERTEX)
        {
            // reset the graph
            g.setVertexNum(num);
            g.reset();
            

        }
        else if(opType == EDGE)
        {
            
            for(unsigned int i=0; i<vect.size(); i++)
            {
                if(!g.addAnEdge(vect[i][0],vect[i][1],errMsg))
                {
                    cerr <<"Error: "<<errMsg <<endl;
                    g.reset();
                    break;
                }
            }
            
            
            // solve by
            solveByApprox1(g);
            
        }
        else
        {
            cerr<<"Illegal Operation Type!"<<endl;
        }
    }
    return 0;
}

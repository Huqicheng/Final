#ifndef GRAPH
#define GRAPH

#include "command.hpp"
#include<string.h>
#include<iostream>
#include <set>
#define MaxVertex 100


class Vertex
{
public:
    int index;
    int parent;
    int d;
    int status;

    Vertex()
    {

    }

    Vertex(const Vertex& C)
    {
        index = C.index;
        parent = C.parent;
        d = C.d;
        status = C.status;
    }
    
};
class Graph
{
private:
    int adjTable[MaxVertex][MaxVertex];
    int vertexNum;


public:

    Graph()
    {
    }
    
    Graph(Graph& g){
        cout << g.vertexNum << endl;
        this->vertexNum = g.vertexNum;
        for(int i=0;i<vertexNum;i++)
            for(int j=0;j<vertexNum;j++){
                this->adjTable[i][j] = g.adjTable[i][j];
            }
        
    }

    Graph(int vertexNum):vertexNum(vertexNum)
    {
    }

    int getVertexNum()
    {
        return vertexNum;
    }

    STATUS beforeEdge();
    STATUS addAnEdge(int startIdx, int endIdx, string& errMsg);
    STATUS shortestPathDijkstra(int v0,string& errMsg);
    STATUS shortestPathBFS(int v0,int target,vector<int>& res,int& dist);
    void getEdges(vector<vector<int> >& edges);
    void getEdges(set<vector<int>>& edges);
    void removeEdge(int i,int j){
        adjTable[i][j] = 0;
        adjTable[j][i] = 0;
    }
    int getIdxOfVertexWithMaxDegree(set<int> setVertices);

    void setVertexNum(int vertexNum);
    void reset();
    void printGraph();



};


#endif // GRAPH

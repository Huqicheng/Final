#include "graph.hpp"
#include<cstdio>
#include<iostream>

#include<queue>
void Graph::setVertexNum(int vertexNum)
{
    this->vertexNum = vertexNum>MaxVertex?MaxVertex:vertexNum;

}

int Graph::getIdxOfVertexWithMaxDegree(set<int> setVertices){
    int maxIdx = -1,maxDegree = -1;
    set<int>::iterator ite;
    for(int i=0;i<vertexNum;i++){
        if((ite = setVertices.find(i)) != setVertices.end()){
            continue;
        }
        int cnt = 0;
        for(int j=0;j<vertexNum;++j){
            if(adjTable[i][j] == 1){
                ++cnt;
            }
        }
        if(cnt > maxDegree){
            maxIdx = i;
            maxDegree = cnt;
        }
    }
    return maxIdx;
}

STATUS Graph::beforeEdge()
{
    for(int i=0; i<this->vertexNum; ++i)
    {
        for(int j=0; j<this->vertexNum; ++j)
        {
            if(this->adjTable[i][j] != 0)
                return FAILED;
        }

    }
    return SUCCESS;
}

void Graph::printGraph()
{
    cout<<"___________________________________________"<<endl;
    for(int i=0; i<this->vertexNum; ++i)
    {
        for(int j=0; j<this->vertexNum; ++j)
        {
            printf("%8d", this->adjTable[i][j] );
        }
        cout<<endl;
    }

    cout<<"___________________________________________"<<endl;
}

void Graph::reset()
{
    memset(this->adjTable,0,MaxVertex*MaxVertex*4);
}


STATUS Graph::addAnEdge(int startIdx,int endIdx, string& errMsg)
{
    if(startIdx>=this->vertexNum || endIdx>=this->vertexNum || endIdx<0 || startIdx<0)
    {
        errMsg = "Index of vertex out of range!";
        return FAILED;
    }
    this->adjTable[startIdx][endIdx] = 1;
    this->adjTable[endIdx][startIdx] = 1;

    return SUCCESS;

}

void Graph::getEdges(vector<vector<int> >& edges){
    for(int i=0;i<vertexNum;++i){
        for(int j=i;j<vertexNum;++j){
            if(adjTable[i][j] == 1){
                vector<int> vect;
                vect.push_back(i);
                vect.push_back(j);
                
                edges.push_back(vect);
            }
        }
    }
}

void Graph::getEdges(set<vector<int>>& edges){
    for(int i=0;i<vertexNum;++i){
        for(int j=i;j<vertexNum;++j){
            if(adjTable[i][j] == 1){
                vector<int> vect;
                vect.push_back(i);
                vect.push_back(j);
                edges.insert(vect);
            }
        }
    }
}



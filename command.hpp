#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <vector>

#define STATUS int
#define SUCCESS 1
#define FAILED 0

using namespace std;

enum operations
{
    VERTEX,
    EDGE,
    SHORTESTPATH
};

typedef enum operations ENUM_OP;


namespace stringutils
{
string trim(const string& str);

STATUS str2Integer(const string& str, int& sum);

int split(const string& str, vector<string>& ret_, string sep);
string replace(const string& str, const string& src, const string& dest);
STATUS parseLine(string& str, int& opType,int& numOfVertices,vector<vector<int> >& points, string& errMsg);

}



#endif // COMMAND_H

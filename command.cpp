#include<iostream>
#include"command.hpp"

string stringutils::trim(const string& str)
{
    string::size_type pos = str.find_first_not_of(' ');
    if (pos == string::npos)
    {
        return str;
    }
    string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

STATUS stringutils::str2Integer(const string& str, int& sum)
{
    sum = 0;
    for(unsigned int i = 0; i<str.length(); ++i)
    {
        if(!isdigit(str[i]))
        {
            return FAILED;
        }
        sum*=10;
        sum+=str[i]-'0';
    }
    return SUCCESS;
}

int stringutils::split(const string& str, vector<string>& ret_, string sep)
{
    if (str.empty())
    {
        return 0;
    }

    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;

    while (pos_begin != string::npos)
    {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos)
        {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty())
        {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}

string stringutils::replace(const string& str, const string& src, const string& dest)
{
    string ret;

    string::size_type pos_begin = 0;
    string::size_type pos       = str.find(src);
    while (pos != string::npos)
    {
        cout <<"replacexxx:" << pos_begin <<" " << pos <<"\n";
        ret.append(str.data() + pos_begin, pos - pos_begin);
        ret += dest;
        pos_begin = pos + 1;
        pos       = str.find(src, pos_begin);
    }
    if (pos_begin < str.length())
    {
        ret.append(str.begin() + pos_begin, str.end());
    }
    return ret;
}


STATUS stringutils::parseLine(string& str, int& opType,int& numOfVertices,vector<vector<int> >& points, string& errMsg)
{
    string s = trim(str);
    if(s.empty())
    {
        errMsg = "Length of command line is 0";
        return FAILED;
    }

    if(s[0] == 'V')
    {
        opType = VERTEX;
        string sub = s.substr(1);
        sub = trim(sub);
        int sum = 0;
        if(!str2Integer(sub,sum))
        {
            errMsg = sub+" cannot be converted to integer";
            return FAILED;
        }
        numOfVertices = sum;

    }
    else if(s[0] == 'E')
    {
        opType = EDGE;
        string sub = s.substr(1);
        sub = trim(sub);

        if(sub[0] != '{' || sub[sub.length()-1] != '}' )
        {
            errMsg = sub+" is illegal";
            return FAILED;
        }
        //cout << s.length()-2 <<endl;
        sub = sub.substr(1,sub.length()-2);
        sub = trim(sub);
        //cout<< sub <<endl;


        vector<string> point;
        if(sub.empty())
        {
            return SUCCESS;
        }
        split(sub,point,"<");
        for(unsigned int i=0; i<point.size(); ++i)
        {
            string trimed = trim(point[i]);
            if(trimed.empty())
            {
                continue;
            }
            int idx = trimed.find('>');
            if(i != point.size()-1 && trimed[trimed.length()-1] != ',')
            {
                errMsg = trimed+" is illegal.";
                return FAILED;
            }

            trimed = trimed.substr(0,idx);
            trimed = trim(trimed);
            vector<string> coords;
            split(trimed,coords,",");
            if(coords.size()!=2)
            {
                errMsg = trimed+" should contain exactly 2 points.";
                return FAILED;
            }
            int coord = 0;
            vector<int> p;
            if(!str2Integer(trim(coords[0]),coord))
            {
                errMsg = coords[0]+" cannot be converted to integer";
                return FAILED;
            }
            else
            {
                p.push_back(coord);
            }

            if(!str2Integer(trim(coords[1]),coord))
            {
                errMsg = coords[1]+" cannot be converted to integer";
                return FAILED;
            }
            else
            {
                p.push_back(coord);
            }

            points.push_back(p);
        }
    }
    
    else
    {
        errMsg = "Illegal command \""+s+"\"";
        return FAILED;
    }
    return SUCCESS;
}

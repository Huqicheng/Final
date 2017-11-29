#pragma once
#include<iostream>
#include <fstream>
using namespace std;



class RandomGen{
private:
    // open /dev/urandom to read
    ifstream urandom;
    
public:
    RandomGen(){
        urandom.open("/dev/urandom");
    }
    
    bool isStreamOpened(){
        if(urandom.fail()){
            return false;
        }
        return true;
    }
    
    unsigned int readUnsignedInt(){
        unsigned int ch;
        urandom.read((char*)&ch, sizeof(unsigned int));
        return (unsigned int)ch;
    }
    
    int readInt(){
        int ch;
        urandom.read((char*)&ch, sizeof(int));
        return (int)ch;
    }
    
    // read an unsigned integer which is less than or equal to upper_bound
    unsigned int readUnsignedIntUpperBound(int upper_bound){
        unsigned int res = readUnsignedInt();
        return res % (upper_bound+1);
        
    }
    
    int readIntUpperBound(int upper_bound){
        int res = readInt();
        //cout << (int)res << endl;
        if(res>0)
            return res % (upper_bound+1);
        else{
            return -((-res)%(upper_bound+1));
        }
        
    }
    
    ~RandomGen(){
        urandom.close();
    }
    
};

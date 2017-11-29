#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include<iostream>
using namespace std;

int main(int argc, char **argv){
    stdout = freopen("input.txt", "w", stdout);
    if(argc>=1){
        argv[0] = (char*)"./graphGen";
    }
    int cnt=0;
    while(cnt<10){
        
    
        execvp("./graphGen",argv);
        
       
            
       

        
        cnt ++;
        
        
    }
    
    sleep(20);
    
    return 0;

    
}

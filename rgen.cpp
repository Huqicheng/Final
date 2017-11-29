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
        
    
        int pid_rgen, pipe_rgen[2];
        pipe(pipe_rgen);
        pid_rgen = fork();
        if(pid_rgen < 0){
            std::cerr << "Error: Failed to fork rgen" << endl;
            exit(1);
        
        }else if(pid_rgen == 0){
            //bind stdout to the write-end of pipe
            dup2(pipe_rgen[1], STDOUT_FILENO);
        
            //close pipes in this process
            close(pipe_rgen[0]);
            close(pipe_rgen[1]);
        
            execvp("./graphGen",argv);
        
            // exit with success
            return 0;
        }
        
       
            //bind file stream read_from_a1 to read-end of pipe_a1
            FILE* py2stream = fdopen(pipe_rgen[0],"r");
            char* line = NULL;
            size_t bytes = 0;
            while(getline(&line, &bytes, py2stream) != -1)
            {
                cout << line;
                
                
            }
            
            //close file stream
            fclose(py2stream);
            
            
            return 0;
            
       

        sleep(2);
        cnt ++;
        close(pipe_rgen[0]);
        close(pipe_rgen[1]);
        
    }
    
    
    
    return 0;

    
}

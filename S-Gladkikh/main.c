#include "stdio.h"
#include "getopt.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/resource.h"
struct GUID{
    int uid;
    int euid;
    int gid;
    int egid;
};


int main(int argc, char *argv[]){
    struct option longOpts []={
        {"Unew_ulimit",optional_argument,0,'U'},
        {"Csize",required_argument,0,'C'},
        {"Vname",required_argument,0,'V'}
    };
    char opt;
    while((opt=getopt_long(argc,argv,"ispucdv",longOpts,NULL))!=-1){
        struct rlimit limit;
        char path[255];
        char **envPtr;
        switch(opt){
            case 'i':
                
                printf("UID:%d\nEUID:%d\nGID:%d\nEGID:%d\n",getuid(),geteuid(),getgid(),getegid());
                break;
            case 's':
                setpgrp();
                break;
            case 'p':
                printf("PID:%d\nPPID:%d\nPGID:%d\n",getpid(),getppid(),getpgid(0));
                break;
            case 'u':
                //getrlimit(RLIMIT_NOFILE,&limit);
                break;
            case 'c':
                getrlimit(RLIMIT_CORE,&limit);
                printf("CORE_FILE soft limit: %lu bytes\nCORE_FILE hard limit: %lu bytes\n",limit.rlim_cur,limit.rlim_max);
                break;
            case 'd':
                getcwd(path,255);
                printf("%s\n",path);
                break;
            case 'v':
                envPtr=environ;
                while(envPtr!=NULL){
                    printf("%s\n",*envPtr);
                    envPtr++;
                }
                break;
            default:break;
        }
    }
    
}
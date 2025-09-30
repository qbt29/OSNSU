#include "stdio.h"
#include "getopt.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/resource.h"
#include "stdlib.h"
struct GUID{
    int uid;
    int euid;
    int gid;
    int egid;
};


int main(int argc, char *argv[]){
    char opt;
    while((opt=getopt(argc,argv,":ispucdvV:C:U:"))!=-1){
        struct rlimit limit;
        char path[255];
        extern char** environ;
        char **envPtr;
        char *ptr;
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
            case 'U':
                getrlimit(RLIMIT_FSIZE,&limit);
                limit.rlim_cur=atol(optarg);
                if(limit.rlim_cur>limit.rlim_max){
                    fprintf(stderr,"Error: ulimit value exceeds hard limit");
                }
                else{
                    setrlimit(RLIMIT_FSIZE,&limit);
                    printf("Set ulimit to %lu",limit.rlim_cur);
                }
                break;
            case 'u':
                getrlimit(RLIMIT_FSIZE,&limit);
                printf("FSIZE soft limit:%lu bytes\nFSIZE hard limit:%lu bytes\n",limit.rlim_cur,limit.rlim_max);
                break;
            case 'c':
                getrlimit(RLIMIT_CORE,&limit);
                printf("CORE soft limit:%lu bytes\nCORE hard limit:%lu bytes\n",limit.rlim_cur,limit.rlim_max);
                break;
            case 'C':
                getrlimit(RLIMIT_CORE,&limit);
                limit.rlim_cur=atol(optarg);
                setrlimit(RLIMIT_CORE,&limit);
                break;
            case 'd':
                getcwd(path,255);
                printf("%s\n",path);
                break;
            case 'v':
                envPtr=environ;
                while(*envPtr!=NULL){
                    printf("%s\n",*envPtr);
                    envPtr++;
                }
                break;
            case 'V':
                ptr = optarg;
                for(;*ptr != '=';ptr++){
                }
                *ptr='\0';
                ptr++;
                setenv(optarg,ptr,1);
                break;
            case '?':
                fprintf(stderr,"Unrecognised option: %s\n",argv[optind-1]);
                break;
            case ':':
                fprintf(stderr,"Missing argument for option \'%s\'\n",argv[optind-1]);
            default:break;
        }
    }
    
}
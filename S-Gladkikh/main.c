#include "stdio.h"
#include "getopt.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
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
        switch(opt){
            case 'i':
                
                printf("UID:%d\nEUID:%d\nGID:%d\nEGID:%d\n",getuid(),geteuid(),getgid(),getegid());
                break;
            default:break;
        }
    }
    
}
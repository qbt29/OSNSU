#include "stdio.h"
#include "getopt.h"
#include "stdlib.h"
#include "string.h"
struct GUID{
    int uid;
    int euid;
    int gid;
    int egid;
};

struct GUID getGUID(void){
    struct GUID out;
    FILE* f = fopen("/proc/self/status","r");
    char buf[255]={'\0'};
    char uid_s[16], euid_s[16], gid_s[16], egid_s[16];
    while(strcmp(buf,"Uid:")!=0){
        fscanf(f,"%s",buf);
    }
    fscanf(f,"%s",uid_s);
    fscanf(f,"%s",euid_s);
    while(strcmp(buf,"Gid:")!=0){
        fscanf(f,"%s",buf);
    }
    fscanf(f,"%s",gid_s);
    fscanf(f,"%s",egid_s);
    fclose(f);
    out.uid=atoi(uid_s);out.euid=atoi(euid_s);out.gid=atoi(gid_s);out.egid=atoi(egid_s);
    return out;


}
int main(int argc, char *argv[]){
    struct option longOpts []={
        {"Unew_ulimit",optional_argument,0,'U'},
        {"Csize",required_argument,0,'C'},
        {"Vname",required_argument,0,'V'}
    };
    char opt = getopt_long(argc,argv,"ispucdv",longOpts,NULL);
    while(opt!=-1){
        switch(opt){
            case 'i':
                struct GUID guid;
                guid = getGUID();
                printf("UID:%d\nEUID:%d\nGID:%d\nEGID:%d\n",guid.uid,guid.euid,guid.gid,guid.egid);
                break;
            default:break;
        }
        opt = getopt_long(argc,argv,"ispucdv",longOpts,NULL);
    }
    
}
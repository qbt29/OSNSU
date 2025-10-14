#include "stdio.h"
#include "unistd.h"
int main(void){
    printf("UID:%d\nEUID:%d\n",getuid(),getuid());
    FILE* f=fopen("file.txt","r");
    if(f==NULL){
        perror("Unable to open file");
    }
    else{
        printf("Success!\n");
        fclose(f);
    }
    setuid(1000);
    printf("UID:%d\nEUID:%d\n",getuid(),geteuid());
    f=fopen("file.txt","r");
    if(f==NULL){
        perror("Unable to open file");
    }
    else{
        printf("Success!\n");
        fclose(f);
    }

}
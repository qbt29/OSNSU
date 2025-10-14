#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct line{
    long begin;
    int len;
}Line;
int main(void){
    Line lines[255];
    int lCnt=0;
    int descr =open("input.txt",O_RDONLY);
    char c=1;
    off_t offset=0;
    while(c != '\0'){
        c='\0';
        lines[lCnt].begin = offset;
        lines[lCnt].len=0;
        while(c != '\n'){
            if(read(descr,&c,1)==0){
                c='\0';
                break;
            }
            lines[lCnt].len++;
        }
        if(c == '\n'){
            lines[lCnt].len--;
        }
        offset = lseek(descr,0,SEEK_CUR);
        lCnt++;
    }
    int op = 0;
    scanf("%d", &op);
    while(op != -1){
        if(op > lCnt || op < 1){
            printf("Line number out of range\n");
        }
        else{
            op--;
            lseek(descr,lines[op].begin,SEEK_SET);
            char buf[255];
            read(descr,buf,lines[op].len);
            buf[lines[op].len]='\0';
            printf("%s\n",buf);
        }
        scanf("%d", &op);
    }
    return 0;

}
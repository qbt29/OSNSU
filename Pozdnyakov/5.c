#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(){

    int fd = open("file.txt", O_RDONLY);

    char str[1024];
    int *mas = NULL;
    int bat;
    int count=2;

    while((bat = read(fd, str, sizeof(str)))>0){
        printf("%d\n",bat);
        for(int i=0;i<bat;i++)
        {
            if(str[i]=='\n')
            {
                if(mas == NULL)
                {
                    mas = (int*)malloc(count*sizeof(int));
                    mas[0] = i;
                    mas[1] = i;
                }
                else
                {
                    count+=2;
                    int *temp = (int*)realloc(mas,count*sizeof(int));
                    mas = temp;
                    mas[count-2] = i-mas[count-3]-1;
                    mas[count-1] = i;
                }
            }
        }
    }

    printf("%d\n",count);

    printf("=========table==========\n");

    for(int i = 0; i<count; i+=2 )
    {
        printf("# %d len = %d, end = %d\n",i/2+1,mas[i],mas[i+1]);
    }
    printf("========================\n");

    while(1)
    {
        int ind;

        if(scanf("%d",&ind)!=1)
        {
            printf("Неверный индекс\n");

            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            continue;
        }
        if(ind == 0)
        {
            printf("end\n");
            break;
        }
        else
        { 
            if((ind-1)<(count/2) && ind>0)
            {   
                if(ind==1)
                {
                    char buf[mas[ind]+1];
                    lseek(fd, 0, SEEK_SET);
                    bat=read(fd,buf,mas[ind-1]);
                    buf[mas[ind-1]]='\0';
                    printf("%s\n",buf);
                }
                else
                {
                    char buf[mas[(ind-1)*2]+1];
                    lseek(fd, mas[(ind-1)*2-1]+1, SEEK_SET);
                    bat = read(fd,buf,mas[(ind-1)*2]);
                    buf[mas[(ind-1)*2]]='\0';
                    printf("%s\n",buf);
                }
            }
            else
            {
                printf("неверный индекс\n");
            }
        }
    }

    close(fd);

    free(mas);

    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/mman.h>
#include <sys/stat.h>

static sigjmp_buf env;

void alarm_handler(int sig) {
    siglongjmp(env, 1);
}

int main(){

    int fd = open("file.txt", O_RDONLY);

    char str[1024];
    int *mas = NULL;
    int bat;
    int count=2;

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    char *mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    for(off_t i = 0; i < sb.st_size; i++)
    {
        if(mapped[i]=='\n')
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

    printf("=========table==========\n");

    for(int i = 0; i<count; i+=2 )
    {
        printf("# %d len = %d, end = %d\n",i/2+1,mas[i],mas[i+1]);
    }
    printf("========================\n");

    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    while(1)
    {
        int ind;
        
        if(sigsetjmp(env, 1) == 0)
        {
            printf("Дано 5 секунд на ввод номера строки\n");
            alarm(5);
            char buffer[100];
            if(fgets(buffer, sizeof(buffer), stdin) != NULL)
            {
                if(sscanf(buffer, "%d", &ind) != 1) {
                    printf("Неверный формат числа\n");
                    continue;
                }

                alarm(0);
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
                            // lseek(fd, 0, SEEK_SET);
                            strncpy(buf,mapped,mas[ind]);
                            // bat=read(fd,buf,mas[ind-1]);
                            buf[mas[ind-1]]='\0';
                            printf("%s\n",buf);
                        }
                        else
                        {
                            char buf[mas[(ind-1)*2]+1];
                            // lseek(fd, mas[(ind-1)*2-1]+1, SEEK_SET);
                            strncpy(buf,mapped+mas[(ind-1)*2-1]+1,mas[(ind-1)*2]);
                            // bat = read(fd,buf,mas[(ind-1)*2]);
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
            else
            {
                printf("Ошибка ввода\n");
            }
        }
        else
        {
            printf("Не успели\n");

            char buf[mas[count-1]+1];
            // lseek(fd, 0, SEEK_SET);
            strncpy(buf,mapped,mas[count-1]+1);
            bat=read(fd,buf,mas[count-1]);
            buf[mas[count-1]]='\0';
            printf("%s\n",buf);

            break;
        }
    }

    close(fd);
    munmap(mapped, sb.st_size);
    free(mas);

    return 0;
}

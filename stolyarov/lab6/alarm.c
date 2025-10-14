#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#include <stdbool.h>

static sigjmp_buf env;

void alarm_handler(int sig) {
    (void)sig;
    siglongjmp(env, 1);
}

int main(){
    int fd = open("file.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char str[1024];
    int *mas = NULL;
    int bat;
    int count = 2;

    // Считываем файл и запоминаем длины строк и позиции '\n'
    while((bat = read(fd, str, sizeof(str))) > 0){
        for(int i = 0; i < bat; i++){
            if(str[i] == '\n'){
                if(mas == NULL){
                    mas = (int*)malloc(count * sizeof(int));
                    if (!mas) { perror("malloc"); close(fd); return 1; }
                    mas[0] = i; // длина первой строки
                    mas[1] = i; // позиция первого '\n'
                } else {
                    count += 2;
                    int *temp = (int*)realloc(mas, count * sizeof(int));
                    if (!temp) { perror("realloc"); free(mas); close(fd); return 1; }
                    mas = temp;
                    mas[count - 2] = i - mas[count - 3] - 1; 
                    mas[count - 1] = i; 
                }
            }
        }
    }

    printf("=========table==========\n");
    for(int i = 0; i < count; i += 2){
        printf("# %d len = %d, end = %d\n", i/2 + 1, mas[i], mas[i+1]);
    }
    printf("========================\n");

    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;            /* без SA_RESTART, чтобы fgets прерывался alarm'ом */
    sigaction(SIGALRM, &sa, NULL);

    bool first_prompt_with_timer = true;   /* <<< таймер только для ПЕРВОГО ввода */

    while(1){
        int ind;

        if(sigsetjmp(env, 1) == 0){
            if (first_prompt_with_timer) {
                printf("Дано 5 секунд на ввод номера строки\n");
                alarm(5);  /* таймер только здесь, один раз */
            } else {
                printf("Введите номер строки (без таймера)\n");
            }

            char buffer[100];
            if (fgets(buffer, sizeof(buffer), stdin) != NULL){
                alarm(0);                 /* на всякий случай снимаем таймер */
                first_prompt_with_timer = false;  /* дальше без таймера */

                if (sscanf(buffer, "%d", &ind) != 1) {
                    printf("Неверный формат числа\n");
                    continue;
                }

                if (ind == 0){
                    printf("end\n");
                    break;
                } else {
                    if ((ind - 1) < (count/2) && ind > 0){
                        if (ind == 1){
                            char buf[mas[ind] + 1];
                            lseek(fd, 0, SEEK_SET);
                            bat = read(fd, buf, mas[ind - 1]);
                            if (bat < 0) { perror("read"); break; }
                            buf[mas[ind - 1]] = '\0';
                            printf("%s\n", buf);
                        } else {
                            char buf[mas[(ind - 1)*2] + 1];
                            lseek(fd, mas[(ind - 1)*2 - 1] + 1, SEEK_SET);
                            bat = read(fd, buf, mas[(ind - 1)*2]);
                            if (bat < 0) { perror("read"); break; }
                            buf[mas[(ind - 1)*2]] = '\0';
                            printf("%s\n", buf);
                        }
                    } else {
                        printf("неверный индекс\n");
                    }
                }
            } else {
                printf("Ошибка ввода\n");
            }
        } else {
            /* Сюда попадаем только при ТАЙМАУТЕ первого ввода */
            printf("Не успели\n");
            first_prompt_with_timer = false;  /* дальше без таймера */
            alarm(0);

            /* Печатаем весь файл и выходим (твоя текущая логика) */
            char buf[mas[count - 1] + 1];
            lseek(fd, 0, SEEK_SET);
            bat = read(fd, buf, mas[count - 1]);
            if (bat < 0) { perror("read"); }
            else {
                buf[mas[count - 1]] = '\0';
                printf("%s\n", buf);
            }
            break;
        }
    }

    close(fd);
    free(mas);
    return 0;
}

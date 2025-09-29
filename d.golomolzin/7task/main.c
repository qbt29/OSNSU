#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


// для взаимодействия с функцией для вывода ошибки используем глобальные переменные
char** global_strs = NULL;
int global_len_strs = 0;

void display_all(int s) {

    printf("\n------- Time's up!!! -------\n");
    for (int i = 0; i < global_len_strs; i++) {
        printf("[%d] %s\n", i, global_strs[i]);
    }
    printf("\n\n");
    return;
};

int main () {

    // максимально возможное кол-во символов в переданной строке
    const size_t MAX_LEN_STR = 1024;
    
    int ind = 0;
    size_t len_strs = 1;
    // массив строк
    char** strs = (char**)malloc(sizeof(char*) * len_strs);
    if (strs == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }

    // инициализируем строку максимально возможной длины
    char* in_str = (char*)malloc(sizeof(char) * MAX_LEN_STR);
    if (in_str == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }

    // считываем с этого файла
    FILE* file = fopen("file.txt", "r");
    while (1) {

        // пока можем считывать, считываем
        if (fgets(in_str, MAX_LEN_STR, file) == NULL) { break; }

        // длина строки
        size_t len_in_str = strlen(in_str);

        // заменяем последний символ
        if (in_str[len_in_str-1] == '\n') {
            in_str[len_in_str-1] = '\0';
            len_in_str--;
        }

        // перезаписываем в новую строку
        char* str = (char*)malloc(sizeof(char) * (len_in_str + 1));
        if (str == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }
        strcpy(str, in_str);

        // если получилось так, что ind выходит за массив, то перевыделяем память
        if (len_strs <= ind) {
            len_strs *= 2;
            char** curr_strs = (char**)malloc(sizeof(char*) * len_strs);
            if (curr_strs == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }

            for (int i = 0; i < ind; i++) {
                curr_strs[i] = strs[i];
            }
            free(strs);
            strs = curr_strs;
        }

        // записываем указатель на строку в общий массив строк И увеличиваем указатель
        strs[ind++] = str;

        // очищаем для последующего использования
        memset(in_str, 0, MAX_LEN_STR * sizeof(char));
    }

    global_strs = strs;
    global_len_strs = ind;

    signal(SIGALRM, display_all);

    int index;
    while (1) {
        scanf("%d", &index);
        
        alarm(5);

        if (index == 0) { break; }
        --index;
        
        if (index == -1 || index >= ind) { break; }
        else {
            printf("%s\n", strs[index]);
        }
        

    }

    // очищаем память
    for (int i = 0; i < ind; i++) {
        free(strs[i]);
    }
    
    free(strs);
    free(in_str);

    return 0;
}
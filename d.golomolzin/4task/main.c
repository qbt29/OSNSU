#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main () {

    // максимально возможное кол-во символов в переданной строке
    const size_t MAX_LEN_STR = 1024;
    
    int ind = 0;
    size_t len_strs = 1;
    char** strs = (char**)malloc(sizeof(char*) * len_strs);
    if (strs == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }

    // инициализируем строку максимально возможной длины
    char* in_str = (char*)malloc(sizeof(char) * MAX_LEN_STR);
    if (in_str == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }

    while (1) {

        // пока можем считывать, считываем
        if (fgets(in_str, MAX_LEN_STR, stdin) == NULL) { break; }

        // длина строки
        size_t len_in_str = strlen(in_str);

        // заменяем последний символ
        if (in_str[len_in_str-1] == '\n') {
            in_str[len_in_str-1] = '\0';
            len_in_str--;
        }
        
        // если начинается с точки => прерываем
        if (len_in_str > 0 && in_str[0] == '.') { break; }

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

    // последовательно выводим строки И сразу очищаем память
    for (int i = 0; i < ind; i++) {
        printf("%s\n", strs[i]);
        free(strs[i]);
    }
    // очищаем память
    free(strs);
    free(in_str);

    return 0;
}
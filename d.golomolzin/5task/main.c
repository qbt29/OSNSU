#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct EL {
    size_t start;   // байт указывающий на начало строки => на след \n
    size_t lenght;  // длина строки = кол-во символов (без \n)
} EL;

int main () {

    // максимально возможное кол-во символов в переданной строке
    const size_t MAX_LEN_STR = 1024;
    size_t pointer = 0;


    // открываем и считываем для создания таблицы отступов
    FILE* file = fopen("file.txt", "r");
    if (file == NULL) { perror("Failed to open file"); return 1; }

    // изначальное кол-во строк
    size_t count_rows = 1;
    // для индексации по таблице
    size_t ind = 0;

    
    EL** rows = (EL**)malloc(sizeof(EL*) * count_rows);
    if (rows == NULL) { perror("Failed to allocate memory"); return 1; }

    // считываем и записываем кол-во строк в файле
    char str[MAX_LEN_STR];
    while (fgets(str, MAX_LEN_STR, file) != NULL) {

        // если получилось так, что ind выходит за массив, то перевыделяем память
        if (count_rows <= ind) {
            // увеличиваем в 2 раза
            count_rows *= 2;

            // перелоцируем память
            EL** curr_rows = (EL**)malloc(sizeof(EL*) * count_rows);
            if (curr_rows == NULL) { perror("Failed to allocate memory"); return 1; }
            
            // перенесем соответсвующие элементы И отчистим массив меньшей длины
            for (int i = 0; i < ind; i++) { curr_rows[i] = rows[i]; }
            free(rows);
            rows = curr_rows;
        }

        // выделяем память для структуры одной строки
        EL* one_row = (EL*)malloc(sizeof(EL));
        if (one_row == NULL) { perror("Failed to allocate memory"); return 1; }

        one_row->start = pointer; // указывает на след \n
        one_row->lenght = strlen(str); // запишем кол-во символов

        // запишем указатель на структуру в массив
        rows[ind++] = one_row;

        // увеличиваем так как добавилось слово
        pointer += one_row->lenght;

    }
    fclose(file);

    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) { perror("ERROR"); return 1; }

    memset(str, 0, MAX_LEN_STR);
    
    while (1) {
        // считываем индекс по которому хотим получить строку
        int index;
        scanf("%d", &index); 
        index--;

        // есил получили 0, значит прерываем работу
        if (index == -1  || index >= ind) {
            break; 
        }
        // иначе можем обработать
        else {
            lseek(fd, rows[index]->start, 0); // сдвигаем дескриптор

            // считываем определенное кол-во байт
            ssize_t bytes = read(fd, str, rows[index]->lenght-1);
            if (bytes > 0) {
                str[rows[index]->lenght] = '\0';
                printf("%s\n", str);
            }
            memset(str, 0, MAX_LEN_STR);
        }
    }

    // чистка
    for (int i = 0; i < ind; i++) { free(rows[i]); }
    free(rows);

    return 0;
}
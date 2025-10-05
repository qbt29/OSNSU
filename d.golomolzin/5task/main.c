#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct EL {
    size_t start;   // [указывает на байт начала строки
    size_t length;  // длина строки в байтах, вместе с \n]
} EL;


int main () {

    // максимально возможное кол-во символов в переданной строке
    const size_t MAX_LEN_STR = 1024;
    // текущая позиция в файле в байтах
    size_t pointer = 0;


    // открываем и считываем для создания таблицы отступов
    FILE* file = fopen("file.txt", "r");
    if (file == NULL) { perror("Failed to open file"); return 1; }

    // изначальное кол-во строк
    size_t count_rows = 1;
    // текущий индекс в массиве структур
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

        one_row->start = pointer; // текущая позиция в файле
        one_row->length = strlen(str); // длина строки вместе с \n

        // запишем указатель на структуру в массив
        rows[ind++] = one_row;

        // увеличиваем так как добавилось слово
        pointer += one_row->length;

    }
    fclose(file);

    // файловый дескриптор - это уникальный числовой идентификатор, который присваивается операционной системой каждому открытому файлу, сокету или другому ресурсу ввода-вывода, такому как канал или устройство

    // предопределенные дескрипторы
    // 0 - STDIN_FILENO  // стандартный ввод
    // 1 - STDOUT_FILENO // стандартный вывод 
    // 2 - STDERR_FILENO // стандартный вывод ошибок (экран)

    int fd = open("file.txt", O_RDONLY);

    // O_WRONLY - только для чтения
    // O_RDONLY - только для записи
    // O_RDWR - для чтения и записи
    // O_CREAT - создание, при отсутствии. Нужно указывать права. =open("file.txt", O_WRONLY | O_CREAT, 0644)
    // O_APPEND - дописывать в конец файла
    // O_TRUNC - обрезать файл до нулевой длины

    if (fd == -1) { perror("ERROR"); return 1; }

    // printf("\n\n%d\n\n", fd);

    memset(str, 0, MAX_LEN_STR);
    
    printf("\nВведите индекс строки, которую вы хотите получить.\n");
    printf("Ввод 0 индекса / индекса превышающего макс.кол-во строк завершит программу\n");
    while (1) {
        // считываем индекс по которому хотим получить строку
        int index;
        scanf("%d", &index); 
        index--;

        // есил получили 0 ИЛИ слишком большой индекс строки, то значит прерываем работу
        if (index == -1  || index >= ind) {
            break; 
        }
        // иначе можем обработать
        else {
            // сдвигаем указатель дескриптора fd на rows[index]->start от начала
            lseek(fd, rows[index]->start, SEEK_SET);

            // SEEK_SET - от начала файла
            // SEET_CUR - от текущего смещения
            // SEET_END - от конца файла

            // считываем определенное кол-во байт
            ssize_t bytes = read(fd, str, rows[index]->length-1);

            if (bytes > 0) {
                str[rows[index]->length] = '\0';
                printf("[%d] : %s\n", index + 1, str);
            }
            memset(str, 0, MAX_LEN_STR);
        }
    }
    printf("\n");
    // чистка
    for (int i = 0; i < ind; i++) { free(rows[i]); }
    free(rows);
    
    return 0;
}
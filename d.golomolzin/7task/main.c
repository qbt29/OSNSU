#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


char* file_inmap = NULL;
size_t size_file_inmap = 0;

typedef struct EL {
    size_t start;   // байт указывающий на начало строки => на след \n
    size_t length;  // длина строки = кол-во символов (без \n)
} EL;

EL** rows = NULL;
size_t ind = 1;


// функция которая выводит все записи из файла. Срабатывает по истечении 5 секунд
void display_all(int s) {

    printf("\nБыл получен сигнал под номером %d (SIGALRM) \n", s);
    printf("\n===== Время вышло!!! =====\n");
    for (size_t i = 0; i != ind; i++) {
        printf("[%d] : %.*s\n", (int)i+1, (int)rows[i]->length-1, file_inmap + rows[i]->start);
    }
    printf("\n");
    exit(0);
}


int main () {

    // максимально возможное кол-во символов в переданной строке
    const size_t MAX_LEN_STR = 1024;
    size_t pointer = 0;

    // открываем и считываем для создания таблицы отступов
    FILE* file = fopen("file.txt", "r");
    if (file == NULL) { perror("Failed to open file"); return 1; }

    size_t count_rows = 1;
    // для индексации по таблице
    ind = 0;

    
    rows = (EL**)malloc(sizeof(EL*) * count_rows);
    if (rows == NULL) { perror("Failed to allocate memory"); return 1; }

    // считываем и записываем кол-во строк в файле
    char str[MAX_LEN_STR];
    while (fgets(str, MAX_LEN_STR, file) != NULL) {

        // если получилось так, что ind выходит за массив, то перевыделяем память
        if (count_rows <= ind) {
            count_rows *= 2;
            EL** curr_rows = (EL**)malloc(sizeof(EL*) * count_rows);
            if (curr_rows == NULL) { perror("Failed to allocate memory"); return 1; }

            for (int i = 0; i < ind; i++) {
                curr_rows[i] = rows[i];
            }
            free(rows);
            rows = curr_rows;
        }

        // выделяем память для структуры одной строки
        EL* one_row = (EL*)malloc(sizeof(EL));
        if (one_row == NULL) { perror("Failed to allocate memory"); return 1; }

        one_row->start = pointer; // указывает на след \n
        one_row->length = strlen(str); // запишем кол-во символов

        // запишем указатель на структуру в массив
        rows[ind++] = one_row;

        // увеличиваем так как добавилось слово
        pointer += one_row->length;

    }
    fclose(file);


    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) { perror("ERROR"); return 1; }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("ERROR IN DETERMINING THE SIZE");
        return 1;
    }
    size_file_inmap = st.st_size;

    // отражает size_file_inmap байтов, начиная со смещения offset файла (или другого объекта), определенного файловым описателем fd, в память, начиная с адреса start.
    file_inmap = mmap(NULL, size_file_inmap, PROT_READ, MAP_PRIVATE, fd, 0);

    // PROT_NONE - доступ к этой области памяти запрещен
    // PROT_EXEC - данные в страницах можно исполнять
    // PROT_READ - данные можно читать
    // PROT_WRITE - в область можно записывать информацию

    if (file_inmap == MAP_FAILED) {
        perror("ERROR SCANNING IN MMAP");
        return 1;
    }

    // асинхронная работа. Если получили SIG... то вызываем функцию display_all
    signal(SIGALRM, display_all);

    // SIGINT    // Ctrl+C (прерывание)
    // SIGTERM   // Запрос на завершение  
    // SIGSEGV   // Segmentation fault
    // SIGUSR1   // Пользовательский сигнал 1

    printf("\nВведите индекс строки, которую вы хотите получить.\n");
    printf("5 секунд бездествия завершают программу и выводит содержимое\n");
    printf("Ввод 0 индекса / индекса превышающего макс.кол-во строк завершит программу\n");
    while (1) {
        // считываем индекс по которому хотим получить строку
        int index;
        scanf("%d", &index); 
        index--;
        
        // отсчитывает переданное кол-во секунд и вызывает SIGALRM
        // возвращает оставшееся время предыдущего таймера
        alarm(5);
        // alarm(0) - сбрасывает счетчик

        // есил получили 0 или слишком большой индекс в таблице, значит прерываем работу
        if (index == -1  || index >= ind) {
            break; 
        }
        else {

            printf("[%d] : %.*s\n", (int)index + 1, (int)rows[index]->length-1, file_inmap + rows[index]->start);
        }

    }

    if (file_inmap != NULL && file_inmap != MAP_FAILED) {
        munmap(file_inmap, size_file_inmap);
    }

    for (int i = 0; i < ind; i++) { free(rows[i]); }
    free(rows);
    close(fd);

    return 0;
}
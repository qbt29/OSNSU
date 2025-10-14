#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define llu unsigned long long

// Функция для отображения идентификаторов процесса
int show_user_identifiers() {
    printf("Текущий реальный ID пользователя: %d\n", getuid());
    printf("Текущий эффективный ID пользователя: %d\n\n", geteuid());
    if (getuid() != geteuid()){
        return 1;  // IDs различаются
    }
    return 0;  // IDs одинаковы
}

// Функция для синхронизации идентификаторов
int sync_user_ids(){
    printf(">>> Синхронизация идентификаторов пользователя <<<\n");
    if (setuid(getuid()) == -1) {
        return 1;  // Ошибка синхронизации
    }
    return 0;  // Успешная синхронизация
}

// Функция для проверки доступа к файлу
void check_file_access(const char *filename){
    printf("Проверка доступа к файлу: '%s'\n", filename);
    FILE *file_ptr = fopen(filename, "r");
    if (file_ptr == NULL) {
        perror("Ошибка доступа к файлу");
    } else {
        printf("Доступ к файлу разрешен!\n");
        fclose(file_ptr);
    }
    printf("\n");
}  

// Главная функция программы
int main(int argc, char *argv[]) {
    // Проверка входных параметров
    if (argc < 2){
        fprintf(stderr, "Формат запуска: %s <путь_к_файлу>\n", argv[0]);
        fprintf(stderr, "Укажите файл для проверки прав доступа\n");
        exit(EXIT_FAILURE);
    }

    const char * target_file = argv[1];

    printf("======= НАЧАЛЬНОЕ СОСТОЯНИЕ =======\n");
    int id_status = show_user_identifiers();
    check_file_access(target_file);
    
    // Если идентификаторы различаются, пробуем синхронизировать
    if (id_status){
        if (sync_user_ids() == 1){
            perror("Сбой при синхронизации ID\n\n");
        } else {
            printf("Идентификаторы синхронизированы!\n\n");
        }
    }
    
    printf("======= КОНЕЧНОЕ СОСТОЯНИЕ =======\n");
    show_user_identifiers();
    check_file_access(target_file);

    return 0;
}
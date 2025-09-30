#include <stdio.h>
#include <errno.h>
#include <unistd.h>


void print_r_and_ef() {
    
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
}

int main() {

    printf("=== До изменения ===\n");
    print_r_and_ef();

    // Первая попытка открыть файл
    FILE* st = fopen("file.txt", "r");
    if (st == NULL) { 
        perror("Ошибка открытия файла (до)"); 
    }
    else {
        printf("Файл успешно открыт (до)\n");
        fclose(st);
    }

    // ИЗМЕНЕНИЕ: устанавливаем эффективный UID в реальный UID
    if (seteuid(getuid()) == -1) {
        perror("Ошибка seteuid");
    }

    printf("\n=== После изменения ===\n");
    print_r_and_ef();

    // Вторая попытка открыть файл
    FILE* fn = fopen("file.txt", "r");
    if (fn == NULL) { 
        perror("Ошибка открытия файла (после)"); 
    }
    else {
        printf("Файл успешно открыт (после)\n");
        fclose(fn);
    }

    return 0;
}
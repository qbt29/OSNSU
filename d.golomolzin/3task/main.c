#include <stdio.h>
#include <errno.h>
#include <unistd.h>


void print_real_efec_id() {
    
    printf("   REAL   UID: %d\n", getuid());
    printf("EFFECTIVE UID: %d\n", geteuid());
}

int main() {

    printf("===== До изменения =====\n");
    print_real_efec_id();

    // первая попытка открыть файл
    FILE* input = fopen("file.txt", "r");
    if (input == NULL) { 
        perror("Ошибка открытия файла (до)"); 
    }
    else {
        printf("Файл успешно открыт (до)\n");
        fclose(input);
    }

    // ИЗМЕНЕНИЕ: устанавливаем эффективный UID в реальный UID
    if (seteuid(getuid()) == -1) {
        perror("Ошибка seteuid");
        return 0;
    }

    printf("\n===== После изменения =====\n");
    print_real_efec_id();

    // вторая попытка открыть файл
    FILE* output = fopen("file.txt", "r");
    if (output == NULL) { 
        perror("Ошибка открытия файла (после)"); 
    }
    else {
        printf("Файл успешно открыт (после)\n");
        fclose(output);
    }

    return 0;
}
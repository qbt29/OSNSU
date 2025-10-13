#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

// Структура для хранения информации о строках
typedef struct {
    long position;
    int length;
} LineInfo;

// Функция открытия файла
int open_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Ошибка открытия файла");
    }
    return fd;
}

int build_line_table(int fd, LineInfo *lines) {
    char buffer[1024];
    long current_pos = 0;
    int bytes_read;
    long line_start = 0;
    int line_count = 0;

    // Перемещаемся в начало файла
    lseek(fd, 0L, SEEK_SET);

    printf("Построение таблицы строк...\n");

    printf("Строка Позиция Длина\n");
    
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                // Сохраняем информацию о строке
                lines[line_count].position = line_start;
                lines[line_count].length = current_pos + i - line_start;
                
                // Для отладки - выводим таблицу строк
                printf("%d: %ld, %d\n", 
                       line_count + 1, lines[line_count].position, lines[line_count].length);
                
                line_count++;
                
                if (line_count >= MAX_LINES) {
                    printf("Достигнут предел количества строк (%d)\n", MAX_LINES);
                    return line_count;
                }
                
                line_start = current_pos + i + 1;
            }
        }
        current_pos += bytes_read;
    }

    // Обработка последней строки (если файл не заканчивается на \n)
    if (line_start < current_pos && line_count < MAX_LINES) {
        lines[line_count].position = line_start;
        lines[line_count].length = current_pos - line_start;
        printf("%d: %ld, %d\n", 
               line_count + 1, lines[line_count].position, lines[line_count].length);
        line_count++;
    }

    printf("Всего строк в файле: %d\n", line_count);
    return line_count;
}

// Функция печати конкретной строки
void print_line(int fd, const LineInfo *lines, int line_count, int line_number) {
    if (line_number < 1 || line_number > line_count) {
        printf("Ошибка: недопустимый номер строки (допустимый диапазон: 1-%d)\n", line_count);
        return;
    }

    int index = line_number - 1;
    
    // Переход к началу строки
    if (lseek(fd, lines[index].position, SEEK_SET) == -1) {
        perror("Ошибка позиционирования");
        return;
    }

    // Чтение строки
    char line_buffer[MAX_LINE_LENGTH];
    int to_read = lines[index].length;
    if (to_read >= MAX_LINE_LENGTH) {
        to_read = MAX_LINE_LENGTH - 1;
    }

    int n = read(fd, line_buffer, to_read);
    if (n == -1) {
        perror("Ошибка чтения");
        return;
    }

    line_buffer[n] = '\0';
    printf("Строка %d: %s\n", line_number, line_buffer);
}

// Функция проверки корректности ввода
int is_valid_input(const char *input) {
    // Проверяем, что ввод состоит только из цифр
    for (int i = 0; input[i] != '\0'; i++) {
        if (!isdigit(input[i])) {
            return 0; // Найден нецифровой символ
        }
    }
    return 1; // Все символы - цифры
}

// Функция интерактивного режима
void interactive_mode(int fd, const LineInfo *lines, int line_count) {
    char input[100];
    int line_number;
    
    printf("\n=== Интерактивный режим ===\n");
    printf("Вводите номера строк для отображения (0 для выхода)\n");
    printf("Доступные строки: 1-%d\n", line_count);
    
    while (1) {
        printf("\nВведите номер строки: ");
        
        // Читаем всю строку ввода
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Ошибка ввода или конец файла\n");
            break;
        }
        
        // Удаляем символ новой строки
        input[strcspn(input, "\n")] = '\0';
        
        // Проверяем на пустой ввод
        if (input[0] == '\0') {
            printf("Ошибка: пустой ввод. Введите номер строки.\n");
            continue;
        }
        
        // Проверяем корректность ввода (только цифры)
        if (!is_valid_input(input)) {
            printf("Ошибка: введите только число (0-%d)\n", line_count);
            continue;
        }
        
        // Преобразуем строку в число
        line_number = atoi(input);
        
        if (line_number == 0) {
            printf("Завершение работы...\n");
            break;
        }

        print_line(fd, lines, line_count, line_number);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        return 1;
    }

    // Открытие файла
    int fd = open_file(argv[1]);
    if (fd == -1) {
        return 1;
    }

    // Создание таблицы строк
    LineInfo lines[MAX_LINES];
    int line_count = build_line_table(fd, lines);
    
    if (line_count == 0) {
        printf("Файл пуст или произошла ошибка\n");
        close(fd);
        return 1;
    }

    // Интерактивный режим
    interactive_mode(fd, lines, line_count);

    close(fd);
    printf("Программа завершена.\n");
    return 0;
}
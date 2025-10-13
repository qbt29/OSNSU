#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <wait.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

static void catchAlarm(int signo);

// Структура для хранения информации о строках
typedef struct {
    long position;
    int length;
} LineInfo;

int FD;
char *FILE_DATA;
size_t FILE_SIZE;
LineInfo LINES[MAX_LINES];
int LINECOUNT;

int open_file(const char *filename) {
    FD = open(filename, O_RDONLY);
    if (FD == -1) {
        perror("Ошибка открытия файла");
        return -1;
    }
    
    // Получаем размер файла
    struct stat sb;
    if (fstat(FD, &sb) == -1) {
        perror("Ошибка получения информации о файле");
        close(FD);
        return -1;
    }
    FILE_SIZE = sb.st_size;
    
    // Отображаем файл в память
    FILE_DATA = mmap(NULL, FILE_SIZE, PROT_READ, MAP_PRIVATE, FD, 0);
    if (FILE_DATA == MAP_FAILED) {
        perror("Ошибка отображения файла в память");
        close(FD);
        return -1;
    }
    
    return FD;
}

// Функция построения таблицы строк с использованием mmap
int build_line_table(char *file_data, size_t file_size, LineInfo *lines) {
    long line_start = 0;
    int line_count = 0;

    printf("Построение таблицы строк...\n");
    printf("Строка Позиция Длина\n");
    
    for (size_t i = 0; i < file_size; i++) {
        if (file_data[i] == '\n') {
            // Сохраняем информацию о строке
            lines[line_count].position = line_start;
            lines[line_count].length = i - line_start;
            

            printf("%d: %ld %d\n", 
                    line_count + 1, 
                    lines[line_count].position, 
                    lines[line_count].length);

            
            line_count++;
            
            if (line_count >= MAX_LINES) {
                printf("Достигнут предел количества строк (%d)\n", MAX_LINES);
                return line_count;
            }
            
            line_start = i + 1;
        }
    }

    // Обработка последней строки (если файл не заканчивается на \n)
    if (line_start < file_size && line_count < MAX_LINES) {
        lines[line_count].position = line_start;
        lines[line_count].length = file_size - line_start;
        printf("%d: %ld, %d\n", 
               line_count + 1, lines[line_count].position, lines[line_count].length);
        line_count++;
    }

    printf("Всего строк в файле: %d\n", line_count);
    return line_count;
}

// Функция печати конкретной строки с использованием mmap
void print_line(char *file_data, const LineInfo *lines, int line_count, int line_number) {
    if (line_number < 1 || line_number > line_count) {
        printf("Ошибка: недопустимый номер строки (допустимый диапазон: 1-%d)\n", line_count);
        return;
    }

    int index = line_number - 1;
    
    // Получаем указатель на начало строки в отображенной памяти
    char *line_start = file_data + lines[index].position;
    int line_length = lines[index].length;
    
    // Выделяем буфер для строки
    char line_buffer[MAX_LINE_LENGTH];
    int to_copy = line_length;
    if (to_copy >= MAX_LINE_LENGTH) {
        to_copy = MAX_LINE_LENGTH - 1;
    }
    
    // Копируем строку из отображенной памяти
    strncpy(line_buffer, line_start, to_copy);
    line_buffer[to_copy] = '\0';
    
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
void interactive_mode(char *file_data, const LineInfo *lines, int line_count) {
    char input[100];
    int line_number;
    
    printf("\n=== Интерактивный режим ===\n");
    printf("Вводите номера строк для отображения (0 для выхода)\n");
    printf("Доступные строки: 1-%d\n", line_count);

    alarm(5);
    signal(SIGALRM, catchAlarm);
    
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

        alarm(0);
        
        // Преобразуем строку в число
        line_number = atoi(input);
        
        if (line_number == 0) {
            printf("Завершение работы...\n");
            break;
        }

        print_line(file_data, lines, line_count, line_number);
        
        // Переустанавливаем таймер для следующего ввода
        alarm(5);
    }
}

void printALLLINES(){
    printf("\nВы не успели ввести данные за отведенное время!\n");
    printf("Вывод всех строк файла:\n");
    for (int i = 1; i <= LINECOUNT; i++){
        print_line(FILE_DATA, LINES, LINECOUNT, i);
    }
}

static void catchAlarm(int signo){
    printALLLINES();
    
    // Освобождаем ресурсы перед выходом
    if (FILE_DATA != MAP_FAILED && FILE_DATA != NULL) {
        munmap(FILE_DATA, FILE_SIZE);
    }
    if (FD != -1) {
        close(FD);
    }
    
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        return 1;
    }

    // Открытие файла и отображение в память
    int fd = open_file(argv[1]);
    if (fd == -1) {
        return 1;
    }

    // Сохраняем глобальные переменные для использования в обработчике сигнала
    FD = fd;
    
    // Создание таблицы строк с использованием отображенной памяти
    int line_count = build_line_table(FILE_DATA, FILE_SIZE, LINES);
    LINECOUNT = line_count;
    
    if (line_count == 0) {
        printf("Файл пуст или произошла ошибка\n");
        munmap(FILE_DATA, FILE_SIZE);
        close(fd);
        return 1;
    }

    // Интерактивный режим
    interactive_mode(FILE_DATA, LINES, line_count);

    // Освобождаем ресурсы
    munmap(FILE_DATA, FILE_SIZE);
    close(fd);
    
    printf("Программа завершена.\n");
    return 0;
}

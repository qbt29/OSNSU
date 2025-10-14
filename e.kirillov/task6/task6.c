#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

typedef struct {
    off_t offset;
    off_t length;
} Line;

typedef struct {
    Line *array;
    int cnt;
    int cap;
} Array;

volatile sig_atomic_t timeout_occurred = 0;

void alarm_handler(int sig) {
    timeout_occurred = 1;
}

void push(Array *a, Line element) {
    if (a->cnt == a->cap) {
        a->cap *= 2;
        a->array = realloc(a->array, a->cap * sizeof(Line));
    }

    a->array[a->cnt++] = element;
}

void printLine(Line line, int file) {
    char *buf = calloc(line.length + 1, sizeof(char));

    lseek(file, line.offset, SEEK_SET);
    read(file, buf, line.length * sizeof(char));

    printf("%s\n", buf);
    free(buf);
}

void print_all_lines_and_exit(Array *table, int file) {
    printf("\n");
    for(int i = 0; i < table->cnt; i++)
        printLine(table->array[i], file);
    
    close(file);
    free(table->array);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2)
        return 1;

    char *path = argv[1];

    Array table;
    table.array = malloc(sizeof(Line));
    table.cnt = 0;
    table.cap = 1;

    int file = open(path, O_RDONLY);
    if (file == -1)
        return 1;

    lseek(file, 0L, SEEK_CUR);

    char c;
    off_t lineOffset = 0;
    off_t lineLength = 0;
    while (read(file, &c, 1) == 1) {
        if (c == '\n') {
            Line current = {lineOffset, lineLength};
            push(&table, current);

            lineOffset += lineLength + 1;
            lineLength = 0;
        } 
        else
            lineLength++;
    }

    if (lineLength > 0) {
        Line current = {lineOffset, lineLength};
        push(&table, current);
    }


    printf("\n\n");
    for(int i = 0; i < table.cnt; ++i){
        printf("offset: %d, len: %d\n",table.array[i].offset, table.array[i].length);
    }
    printf("\n\n");

    // Установка обработчика сигнала
    signal(SIGALRM, alarm_handler);

    // Сохраняем оригинальные флаги stdin
    int old_flags = fcntl(STDIN_FILENO, F_GETFL);
    // Устанавливаем неблокирующий режим
    fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);
    alarm(5);
    while (1) {
        timeout_occurred = 0;
        printf("Enter the line number: ");
        fflush(stdout);

        char temp[123] = {0};
        int pos = 0;
        
        // Чтение посимвольно с проверкой таймаута
        while (pos < sizeof(temp) - 1) {
            if (timeout_occurred) {
                print_all_lines_and_exit(&table, file);
            }
            
            char ch;
            int bytes_read = read(STDIN_FILENO, &ch, 1);
            
            if (bytes_read > 0) {
                if (ch == '\n') {
                    temp[pos] = '\0';
                    break;
                }
                temp[pos++] = ch;
            } else if (bytes_read == 0) {
                // EOF
                break;
            } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                // Ошибка чтения
                break;
            }
            
            // Небольшая задержка чтобы не грузить CPU
            usleep(10000); // 10ms
        }

        // Отмена таймера
        alarm(0);

        // Если ничего не введено и был таймаут
        if (timeout_occurred) {
            print_all_lines_and_exit(&table, file);
        }

        // Если ничего не введено
        if (pos == 0) {
            break;
        }

        int num = atoi(temp);

        if(num == 0)
            break;
        
        if (table.cnt < num) {
            printf("The file contains only %d line(s).\n", table.cnt);
            continue;
        }

        Line line = table.array[num - 1];
        printLine(line, file);
    }

    // Отмена таймера перед выходом
    alarm(0);
    // Восстанавливаем оригинальные флаги
    fcntl(STDIN_FILENO, F_SETFL, old_flags);

    close(file);
    free(table.array);

    return 0;
}
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

typedef struct {
    off_t offset;
    off_t length;
} Line;

typedef struct {
    Line *array;
    int cnt;
    int cap;
} Array;


void push(Array *a, Line element) {
    if (a->cnt == a->cap) {
        a->cap *= 2;
        a->array = realloc(a->array, a->cap * sizeof(Line));
    }

    a->array[a->cnt++] = element;
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

    while (1) {

        char temp[123];
        printf("Enter the line number: ");
        scanf("%s", temp);
        int num = atoi(temp);
        if(atoi(temp) == 0)
            break;
        
        if (table.cnt < num) {
            printf("The file contains only %d line(s).\n", table.cnt);
            continue;
        }

        Line line = table.array[num - 1];
        char *buf = calloc(line.length + 1, sizeof(char));

        lseek(file, line.offset, SEEK_SET);
        read(file, buf, line.length * sizeof(char));

        printf("%s\n", buf);
        free(buf);
    }


    close(file);
    free(table.array);
    table.array = NULL;
    table.cnt = 0;
    table.cap = 0;

    return 0;
}
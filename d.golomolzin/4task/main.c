#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// cтруктура узла списка
typedef struct Node {
    char* data;
    struct Node* next;
} Node;


int main () {

    // максимально возможное кол-во символов в переданной строке
    const size_t MAX_LEN_STR = 1024;
    
    // инициализируем голову и хвост списка
    Node* head = NULL;
    Node* tail = NULL;

    // инициализируем строку максимально возможной длины
    char* in_str = (char*)malloc(sizeof(char) * MAX_LEN_STR);
    if (in_str == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }

    printf("\nВведите строки для записи в список.\n");
    printf("Строка с начальным символом <.> завершает ввод и выводит все содержимое:\n");

    while (1) {

        // пока можем считывать, считываем
        if (fgets(in_str, MAX_LEN_STR, stdin) == NULL) { break; }

        // длина строки
        size_t len_in_str = strlen(in_str);

        // заменяем последний символ
        if (in_str[len_in_str-1] == '\n') {
            in_str[len_in_str-1] = '\0';
            len_in_str--;
        }
        
        // если начинается с точки => прерываем
        if (len_in_str > 0 && in_str[0] == '.') { break; }

        // перезаписываем в новую строку
        char* str = (char*)malloc(sizeof(char) * (len_in_str + 1));
        if (str == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }
        strcpy(str, in_str);

        // создаем новый узел списка
        Node* new_node = (Node*)malloc(sizeof(Node));
        if (new_node == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }
        
        new_node->data = str;
        new_node->next = NULL;
        
        // добавляем узел в конец списка
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }

        // очищаем для последующего использования
        memset(in_str, 0, MAX_LEN_STR * sizeof(char));
    }

    printf("\nСодержимое списка:\n");
    // последовательно выводим строки из списка И сразу очищаем память
    Node* current = head;
    while (current != NULL) {

        printf("%s\n", current->data);
        Node* to_free = current;
        current = current->next;
        
        // очищаем память
        free(to_free->data);
        free(to_free);
    }
    printf("\n");

    free(in_str);

    return 0;
}
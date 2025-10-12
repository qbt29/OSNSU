#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// структура узла списка
typedef struct Node {
    char* data;
    struct Node* next;
} Node;


// удаление стремных символов
void delete_wrong_sumbols(char* str)
{
    int i = 0, j = 0;
    int in_escape = 0;
    int escape_start = -1;
    
    while (str[i] != '\0')
    {
        if (in_escape)
        {
            // Обработка случая ^[O - удаляем следующий символ
            if (str[i] == 'O' && i > 0 && str[i-1] == 27)
            {
                // Пропускаем текущий 'O' и следующий символ
                in_escape = 0;
                i++; // пропускаем 'O'
                if (str[i] != '\0') {
                    i++; // пропускаем следующий символ после 'O'
                }
                continue;
            }
            else if ((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || str[i] == '~')
            {
                in_escape = 0;
            }
        }
        else if (str[i] == 27)
        { // ESC символ (^[)
            in_escape = 1;
            escape_start = i;
        }
        else if (((str[i] < 128) || (str[i] >= 192 && str[i] <= 253)))
        {
            if (str[i] == '[' && i > 0 && str[i-1] == 27) {}
            else { str[j++] = str[i]; }
        }
        else if ((str[i] >= 128 && str[i] <= 191)) { str[j++] = str[i]; }
        else if (str[i] >= 32 && str[i] <= 126) { str[j++] = str[i]; }
        
        i++;
    }
    str[j] = '\0';
}


int main() {
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
        printf("Ваша строка: ");
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

        // удаление символов
        // delete_wrong_sumbols(in_str);
        
        // обновляем длину строки после фильтрации
        len_in_str = strlen(in_str);

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

    printf("┌──────────────────────────┐\n");
    printf("│    Содержимое списка     │\n");
    printf("└──────────────────────────┘\n");

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
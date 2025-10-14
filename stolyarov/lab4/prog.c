#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// структура узла списка
typedef struct Node {
    char* data;
    struct Node* next;
} Node;


void delete_wrong_sumbols(char* str)
{
    int i = 0, j = 0;
    int in_escape = 0;
    int escape_start = -1;
    
    while (str[i] != '\0')
    {
        if (in_escape)
        {
            if (str[i] == 'O' && i > 0 && str[i-1] == 27)
            {
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
        { 
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
    const size_t MAX_LEN_STR = 1024;

    Node* head = NULL;
    Node* tail = NULL;

    char* in_str = (char*)malloc(sizeof(char) * MAX_LEN_STR);
    if (in_str == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }

    printf("\nВведите строки для записи в список.\n");
    while (1) {
        printf("Ваша строка: ");
        if (fgets(in_str, MAX_LEN_STR, stdin) == NULL) { break; }

        size_t len_in_str = strlen(in_str);

        if (in_str[len_in_str-1] == '\n') {
            in_str[len_in_str-1] = '\0';
            len_in_str--;
        }

        if (len_in_str > 0 && in_str[0] == '.') { break; }
        
        len_in_str = strlen(in_str);

        char* str = (char*)malloc(sizeof(char) * (len_in_str + 1));
        if (str == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }
        strcpy(str, in_str);

        Node* new_node = (Node*)malloc(sizeof(Node));
        if (new_node == NULL) { fprintf(stderr, "Ошибка выделения памяти!\n"); return 1; }

        new_node->data = str;
        new_node->next = NULL;

        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }

        memset(in_str, 0, MAX_LEN_STR * sizeof(char));
    }

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

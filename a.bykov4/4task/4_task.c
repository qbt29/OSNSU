#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структура для узла списка
struct Node {
    char *data;
    struct Node *next;
};

// Функция для создания нового узла
struct Node* createNode(const char *str) {
    // Выделяем память под узел
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Ошибка выделения памяти для узла\n");
        return NULL;
    }
    // Выделяем память под строку (+1 для нулевого символа)
    newNode->data = (char*)malloc(strlen(str) + 1);
    if (newNode->data == NULL) {
        printf("Ошибка выделения памяти для строки\n");
        free(newNode);
        return NULL;
    }
    // Копируем строку
    strcpy(newNode->data, str);
    newNode->next = NULL;
    return newNode;
}

// Функция для добавления узла в конец списка
void appendNode(struct Node **head, const char *str) {
    struct Node *newNode = createNode(str);
    if (newNode == NULL) {
        return;
    }
    
    if (*head == NULL) {
        // Если список пуст, новый узел становится головой
        *head = newNode;
    }
    else {
        // Ищем последний узел
        struct Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        // Добавляем новый узел в конец
        current->next = newNode;
    }
}

// Функция для вывода всего списка
void printList(struct Node *head) {
    struct Node *current = head;
    printf("\nСодержимое списка:\n");
    while (current != NULL) {
        printf("%s", current->data);
        current = current->next;
    }
}

// Функция для освобождения памяти списка
void freeList(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free(temp->data); // Освобождаем память строки
        free(temp);       // Освобождаем память узла
    }
}

int main() {
    struct Node *head = NULL; // Голова списка
    char buffer[1024];        // Буфер для ввода строк

    printf("Введите строки (для завершения введите точку в начале строки):\n");
    
    while (1) {
        // Читаем строку с клавиатуры
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Ошибка чтения ввода\n");
            break;
        }
        // Проверяем, не введена ли точка в начале строки
        if (buffer[0] == '.') {
            break;
        }
        
        // Добавляем строку в список
        appendNode(&head, buffer);
    }
    
    // Выводим все строки из списка
    printList(head);
    
    // Освобождаем память
    freeList(head);
    
    return 0;
}
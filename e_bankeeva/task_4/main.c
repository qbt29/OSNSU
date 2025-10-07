#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char *str;
    struct Node *next;
} Node;

int main()
{
    char input_str[512];
    Node *head = NULL;
    Node *tail = NULL;

    while (fgets(input_str,512, stdin))
    {
        if (input_str[0] == '.')
            break;

        const int len = strlen(input_str);
        if (input_str[len - 1] == '\n' && len > 0)
        {
            input_str[len - 1] = '\0';
        }

        char *str = malloc((len + 1));
        strcpy(str, input_str);

        Node *new_node = malloc(sizeof(Node));
        new_node->str = str;
        new_node->next = NULL;

        if (head == NULL)
        {
            head = new_node;
            tail = new_node;
        }
        else
        {
            tail->next = new_node;
            tail = new_node;
        }
    }

    printf("\n");

    for (Node *node = head; node != NULL; node = node->next)
    {
        printf("%s\n", node->str);
    }

    Node *node = head;
    while (node != NULL)
    {
        Node *temp = node;
        node = node->next;
        free(temp->str);
        free(temp);
    }
    return 0;
}

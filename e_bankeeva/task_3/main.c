#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


FILE* open_file()
{
    FILE *f = fopen("file.txt", "r");
    if (f == NULL)
    {
        perror("File opening error\n");
        exit(1);
    }

    printf("File is opened\n");
    return f;
}


void close_file(FILE* f)
{
    if (fclose(f) != 0)
    {
        perror("File closing error\n");
        exit(1);
    }

    printf("File is closed\n");
}


int main()
{
    printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());

    FILE* f = open_file();
    close_file(f);
    f = NULL;

    if (getuid() != geteuid())
    {
        if (setuid(getuid()) != 0)
        {
            perror("Setuid failed\n");
            return 1;
        }

        printf("Real UID and effective UID have become the same value\n");
    }

    printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());

    f = open_file();
    close_file(f);

    return 0;
}

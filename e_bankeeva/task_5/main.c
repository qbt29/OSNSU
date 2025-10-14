#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>


int main(int argc, const char * argv[])
{
    const int file = open(argv[1], O_RDONLY);
    if (file == -1)
        return -1;

    off_t start[1024], pos = 0;
    int lens[1024];
    char val;
    int len = 0, str_count = 0;

    while (read(file, &val, 1) == 1)
    {
        if (len == 0)
            start[str_count] = pos;

        len += 1;
        pos += 1;

        if (val == '\n')
        {
            lens[str_count] = len;
            len = 0;
            str_count += 1;
        }
    }

    if (len > 0 && str_count < 1024)
    {
        lens[str_count] = len;
        str_count += 1;
    }

    printf("Line | Offset | Length\n\n");
    for (int i = 0; i < str_count; i++)
        printf("%d\t%lld\t%d\n", i + 1, (long long)start[i], lens[i] - 1);


    int n;
    char rez[1024];
    printf("\nВведите номер строки\n");
    while (1)
    {
        if (scanf("%d", &n) == 1 && n > 0 && n < str_count + 1)
        {
            if (n > str_count || n < 0)
                return -1;

            const off_t st = start[n - 1];
            const int l = lens[n - 1];

            lseek(file, st, SEEK_SET);
            read(file, rez, l);
            rez[l] = '\0';
            printf("%s", rez);
        }
        else
        {
            printf("Введите число - номер существующей строки\n");
            while (getchar() != '\n');
        }

        if (n == 0)
            break;
    }

    close(file);

    return 0;
}
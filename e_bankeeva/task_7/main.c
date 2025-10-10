#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>



volatile int timeout = 0;

void alarm_handler()
{
    printf("Time's up\n");
    timeout = 1;
    fclose(stdin);
}

int main(int argc, const char * argv[])
{
    signal(SIGALRM, alarm_handler);

    const int file = open(argv[1], O_RDONLY);
    if (file == -1)
        return -1;

    struct stat st;
    if (fstat(file, &st) == -1)
    {
        printf("Error stating file\n");
        close(file);
        return -1;
    }
    const off_t filesize = st.st_size;

    const char *data = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, file, 0);
    if (data == MAP_FAILED)
    {
        printf("Error mapping file\n");
        close(file);
        return -1;
    }

    off_t start[1024], pos = 0;
    int lens[1024];
    int len = 0, str_count = 0;

    while (pos < filesize)
    {
        char val = data[pos];

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

    printf("Line Offset length\n");
    for (int i = 0; i < str_count; i++)
        printf("%d\t%lld\t%d\n", i + 1, (long long)start[i], lens[i] - 1);


    int n, f = 0;
    char rez[1025];

    while (1)
    {
        if (f == 0)
            alarm(5);
        if (scanf("%d", &n) == 1 && n > 0 && n < str_count + 1)
        {
            if (f == 0)
            {
                alarm(0);
                f = 1;
                if (timeout == 1)
                {
                    break;
                }
            }

            if (n > str_count || n < 0)
                return -1;

            const off_t start_ = start[n - 1];
            const int l = lens[n - 1];

            memcpy(rez, data + start_, l);
            rez[l] = '\0';
            printf("%s", rez);
        }
        else
        {
            if (timeout == 1 || n == 0)
                break;

            printf("Введите число - номер существующей строки\n");
            while (getchar() != '\n');
        }
    }

    if (timeout == 1)
    {
        for (int i = 0; i < str_count; i++) {
            memcpy(rez, data + start[i], lens[i]);
            rez[lens[i]] = '\0';
            printf("%s", rez);
        }
    }

    munmap((void *)data, filesize);
    close(file);

    return 0;
}

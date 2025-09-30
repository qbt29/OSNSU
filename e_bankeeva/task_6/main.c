#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


volatile int timeout = 0;

void alarm_handler()
{
    printf("Time's up\n");
    timeout = 1;
}

int main(int argc, const char * argv[])
{
    const int file = open(argv[1], O_RDONLY);
    if (file == -1)
    {
        printf("Error opening file\n");
        return -1;
    }

    signal(SIGALRM, alarm_handler);

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

    int n;
    char rez[1024];

    while (timeout == 0)
    {
        alarm(5);
        scanf("%d", &n);
        alarm(0);
        if (timeout == 1)
            break;

        if (n > str_count)
        {
            printf("No such line\n");
            return -1;
        }

        const off_t st = start[n - 1];
        const int l = lens[n - 1];

        lseek(file, st, SEEK_SET);
        read(file, rez, l);
        rez[l] = '\0';
        printf("%s", rez);
    }

    close(file);

    return 0;
}

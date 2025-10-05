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

    int n;
    char rez[1024];
    scanf("%d", &n);

    if (n > str_count)
        return -1;

    while (n != 0)
    {
        const off_t st = start[n - 1];
        const int l = lens[n - 1];

        lseek(file, st, SEEK_SET);
        read(file, rez, l);
        rez[l] = '\0';
        printf("%s", rez);

        scanf("%d", &n);
        if (n > str_count)
            return -1;
    }

    close(file);

    return 0;
}
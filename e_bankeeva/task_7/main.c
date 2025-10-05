#include <stdio.h>
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
}


int main(int argc, const char * argv[])
{
    const int file = open(argv[1], O_RDONLY);
    if (file == -1)
    {
        printf("Error opening file\n");
        return -1;
    }

    struct stat st;
    if (fstat(file, &st) == -1)
    {
        printf("Error stating file\n");
        return -1;
    }

    const off_t filesize = st.st_size;

    const char *data = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, file, 0);
    if (data == MAP_FAILED)
    {
        printf("Error mapping file\n");
        return -1;
    }

    off_t start[1024], pos = 0;
    int lens[1024], n, len = 0, str_count = 0;
    char rez[1024];

    signal(SIGALRM, alarm_handler);

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

        const off_t sta = start[n - 1];
        const int l = lens[n - 1];

        lseek(file, sta, SEEK_SET);
        read(file, rez, l);
        rez[l] = '\0';
        printf("%s", rez);
    }

    close(file);

    return 0;
}

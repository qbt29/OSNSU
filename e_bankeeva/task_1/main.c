#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/resource.h>

extern char **environ;


int main(int argc, char *argv[])
{
    int option_index = 0;
    int c;
    struct rlimit data;
    char buffer[256];
    char **env = environ;

    struct option long_options[] ={
        {"i", no_argument,0, 'i'},
        {"s", no_argument,0, 's'},
        {"p", no_argument, 0, 'p'},
        {"u", no_argument, 0, 'u'},
        {"U", required_argument, 0, 'U'},
        {"c", no_argument, 0, 'c'},
        {"C", required_argument, 0, 'C'},
        {"d", no_argument, 0, 'd'},
        {"v", no_argument, 0, 'v'},
        {"V", required_argument, 0, 'V'}
    };

    while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                printf("Real UID: %d, Real GID: %d\n", getuid(), getgid());
                printf("Effective UID: %d, Effective GID: %d\n", geteuid(), getegid());
                break;
            case 's':
                if (setpgid(0, 0) == 0)
                    printf("Process became the group leader");
                break;
            case 'p':
                printf("PID: %d\n", getpid());
                printf("Parent PID: %d\n", getppid());
                printf("Process group ID: %d\n", getpgrp());
                break;
            case 'u':
                getrlimit(RLIMIT_NOFILE, &data);
                printf("Ulimit value:\nSoft limit: %llu. Hard limit: %llu.\n", data.rlim_cur, data.rlim_max);
                break;
            case 'U':
                if (argc < 4) {
                    printf("No arguments");
                    break;
                }

                getrlimit(RLIMIT_NOFILE, &data);

                const long long soft_value = strtoll(argv[2], NULL, 10);
                data.rlim_cur = soft_value;

                const long long hard_value = strtoll(argv[3], NULL, 10);
                data.rlim_max = hard_value;

                printf("Ulimit value changed:\nSoft limit: %llu. Hard limit: %llu.\n", soft_value, hard_value);
                break;
            case 'c':
                getrlimit(RLIMIT_CORE, &data);
                printf("Maximum core-file size %llu\n", data.rlim_max);
                break;
            case 'C':
                if (argc < 3) {
                    printf("No arguments");
                    break;
                }

                getrlimit(RLIMIT_CORE, &data);
                long long const max_value = strtoll(argv[2], NULL, 10);
                data.rlim_cur = max_value;

                printf("Changed core file size to %llu.\n", max_value);
                break;
            case 'd':
                getcwd(buffer, sizeof(buffer));
                printf("Current working directory: %s\n", buffer);
                break;
            case 'v':
                while (*env)
                    printf("%s\n", *env++);
                break;
            case 'V':
                if (argc < 5)
                    printf("No arguments");

                setenv(argv[2], argv[4], 1);
                printf("Added environment variable %s=%s\n", argv[2], argv[4]);
                break;
            case '?':
                printf("Unknown option\n");
                break;
        }
    }

    return 0;
}
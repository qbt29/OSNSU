#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
extern char **environ;
extern char *optarg;

int main(int argc, char *argv[]) 
{
    int opt;

    // Пока есть ещё опции (возвращает букву найденной опции)
    while ((opt = getopt(argc, argv, "ipvV:suU:cC:d")) != -1) 
    {
        switch (opt) 
        {
            case 'i':
                printf("UID=%d EUID=%d GID=%d EGID=%d\n", getuid(), geteuid(), getgid(), getegid());
                break;
            case 's':
                if (setpgid(0, 0) != 0)
                    perror("setpgid");
                else
                    printf("Процесс стал лидером группы\n");
                break;
            case 'p':
                printf("PID=%d PPID=%d PGID=%d\n", getpid(), getppid(), getpgrp());
                break;
            case 'u': 
            {
                struct rlimit rl;
                if (getrlimit(RLIMIT_CPU, &rl) != 0) {
                    perror("getrlimit");
                } else {
                    printf("Текущий ulimit (CPU time) = %ld\n", rl.rlim_cur);
                }
                break;
            }
            case 'U': 
            {
                if (!optarg) 
                {
                    fprintf(stderr, "Ошибка: -U требует аргумент\n");
                    break;
                }
                long new_limit = atol(optarg);
                struct rlimit rl;
                if (getrlimit(RLIMIT_CPU, &rl) != 0) 
                {
                    perror("getrlimit");
                    break;
                }
                rl.rlim_cur = new_limit;
                if (setrlimit(RLIMIT_CPU, &rl) != 0) 
                {
                    perror("setrlimit");
                } else 
                {
                    printf("ulimit изменён на %ld\n", new_limit);
                }
                break;
            }
            case 'c': 
            {
                struct rlimit rl;
                if (getrlimit(RLIMIT_CORE, &rl) != 0)
                    perror("getrlimit");
                else
                    printf("Максимальный размер core-файла = %ld\n", rl.rlim_cur);
                break;
            }
            case 'C': 
            {
                if (!optarg) 
                {
                    fprintf(stderr, "Ошибка: -C требует аргумент\n");
                    break;
                }
                long new_size = atol(optarg);
                struct rlimit rl;
                if (getrlimit(RLIMIT_CORE, &rl) != 0) 
                {
                    perror("getrlimit");
                    break;
                }
                rl.rlim_cur = new_size;
                if (setrlimit(RLIMIT_CORE, &rl) != 0)
                    perror("setrlimit");
                else
                    printf("Размер core-файла изменён на %ld\n", new_size);
                break;
            }
            case 'd': 
            {
                char cwd[1024];   // буфер для пути
                if (getcwd(cwd, sizeof(cwd)) != NULL) 
                {
                    printf("Текущая директория: %s\n", cwd);
                } else 
                {
                    perror("getcwd"); // если ошибка
                }
                break;
            }
            case 'v': {
                for (char **env = environ; *env != NULL; env++)
                    printf("%s\n", *env);
                break;
            }

            case 'V': {
                if (!optarg) {
                    fprintf(stderr, "Ошибка: опция -V требует аргумент name=value\n");
                    break;
                }
                char *eq = strchr(optarg, '=');
                if (!eq) {
                    fprintf(stderr, "Ошибка: аргумент должен быть в формате name=value\n");
                    break;
                }
                *eq = '\0';
                char *name = optarg;
                char *value = eq + 1;
                if (setenv(name, value, 1) != 0)
                    perror("setenv");
                break;
            }
            default:
                printf("Неизвестная опция: %c\n", opt);
        }

    }
    return 0;
}

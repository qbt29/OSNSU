#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

extern char *optarg;
extern char **environ;

void print_user_ids(){
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
    printf("Real GID: %d\n", getgid());
    printf("Effective GID: %d\n", getegid());
}

int group_leader(){
    // 0 if complete, -1 if failed
    if (setpgid(0, 0) == -1) {
        perror("group leader set failed");
        return -1;
    }
    else {
        printf("Process became group leader\n");
    }

    return 0;
}

void print_process_id(){
    printf("Process ID: %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());
    printf("Process Group ID: %d\n", getpgrp());
}

int print_ulimit(){
    struct rlimit limit;

    // Получить текущие лимиты
    if (getrlimit(RLIMIT_FSIZE, &limit) == 0) {
        printf("Current ulimit: %ld\n", limit.rlim_cur);
    }
    else {
        perror("getrlimit");
        return -1;
    }
    
    return 0;
}

int change_ulimit(const char *value){
    struct rlimit limit;

    int new_limit = atoi(value);

    if (new_limit <= 0){
        perror("Incorrect value");
        return -1;
    }

    limit.rlim_cur = new_limit;
    limit.rlim_max = new_limit;

    if (setrlimit(RLIMIT_FSIZE, &limit) == -1) {
        perror("getrlimit");
        return -1;
    }

    printf("New limit set\n");

    return 0;
}

int print_core_size(){
    struct rlimit limit;

    // Получить текущие лимиты
    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
        printf("Current core size: %ld\n", limit.rlim_cur);
    }
    else {
        perror("Core_size");
        return -1;
    }
    
    return 0;
}

int change_core_size(const char *value){
    struct rlimit limit;

    int new_limit = atoi(value);

    if (new_limit <= 0){
        perror("Incorrect value");
        return -1;
    }

    limit.rlim_cur = new_limit;
    limit.rlim_max = new_limit;

    if (setrlimit(RLIMIT_CORE, &limit) == -1) {
        perror("getrlimit");
        return -1;
    }

    printf("New core size set\n");

    return 0;

}

int print_directory(){
    char currentDirectory[1024]; // Буфер для хранения пути

    if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL) {
        printf("Current directory: %s\n", currentDirectory);
    }
    else {
        perror("Error with dir\n");
        return -1;
    }
    return 0;
}

void print_environment(){
    printf("Environment variables:\n");
    for (char **env = environ; *env != NULL; env++) {
        printf("    %s\n", *env);
    }
}

int set_environment(const char *assignment){
    char *equals = strchr(assignment, '=');
    if (equals == NULL || equals == assignment){
        perror("set_env assigment");
        return -1;
    }

    size_t name_len = equals - assignment;
    char name[name_len + 1];
    strncpy(name, assignment, name_len);
    name[name_len] = '\0';

    const char *value = equals + 1;

    if (setenv(name, value, 1) == -1) {
        perror("setenv failed");
        return -1;
    }
    
    printf("Set environment variable: %s=%s\n", name, value);
    return 0; 
}

int main(int argc, char *argv[]){
    char options[ ] = "ispucdvU:C:V:";  /* valid options */
    int c;
    int invalid = 0;
    
    char **reversed_argv = malloc((argc + 1) * sizeof(char *));
    if (!reversed_argv) {
        perror("malloc failed");
        return 1;
    }
    
    // Копируем название программы
    reversed_argv[0] = argv[0];
    
    // остальные элементы помещаем в обратном порядке
    for (int i = 1; i < argc; i++) {
        reversed_argv[i] = argv[argc - i];
    }
    reversed_argv[argc] = NULL;

    while ((c = getopt(argc, reversed_argv, options)) != -1) {
        switch (c) {
            case 'i':
                print_user_ids();
                break;
            case 's':
                if (group_leader() == -1){
                    return 1;
                }
                break;
            case 'p':
                print_process_id();
                break;
            case 'u':
                if (print_ulimit() == -1){
                    return 1;
                }
                break;
            case 'U':
                if (change_ulimit(optarg) == -1){
                    return 1;
                }
                break;
            case 'c':
                if (print_core_size() == -1){
                    return 1;
                }
                break;
            case 'C':
                if (change_core_size(optarg) == -1){
                    return 1;
                }
                break;
            case 'd':
                if (print_directory() == -1){
                    return 1;
                }
                break;
            case 'v':
                print_environment();
                break;
            case 'V':
                if (set_environment(optarg) == -1){
                    return 1;
                }
                break;
            case '?':
                printf("invalid option\n");
                invalid++;
                break;
        }
    }
    
    return 0;
}
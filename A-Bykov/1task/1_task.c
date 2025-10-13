#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>

extern char *optarg;
extern char **environ;
extern int optind, opterr, optopt;

void print_user_ids(){
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
    printf("Real GID: %d\n", getgid());
    printf("Effective GID: %d\n", getegid());
}

int group_leader(){
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

    if (getrlimit(RLIMIT_FSIZE, &limit) == 0) {
        printf("Current ulimit: %ld\n", limit.rlim_cur);
        printf("Max ulimit: %ld\n", limit.rlim_max);
    }
    else {
        perror("getrlimit");
        return -1;
    }
    
    return 0;
}

int change_ulimit(const char *value){
    struct rlimit limit;

    long new_limit = atol(value);

    if (new_limit <= 0){
        fprintf(stderr, "Invalid ulimit value: %s\n", value);
        return -1;
    }

    limit.rlim_cur = new_limit;
    limit.rlim_max = new_limit;

    if (setrlimit(RLIMIT_FSIZE, &limit) == -1) {
        perror("setrlimit");
        return -1;
    }

    printf("New limit set to %ld\n", new_limit);

    return 0;
}

int print_core_size(){
    struct rlimit limit;

    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
        printf("Current core size: %lu\n", limit.rlim_cur);
        printf("Max core size: %lu\n", limit.rlim_max);
    }
    else {
        perror("Core_size");
        return -1;
    }
    
    return 0;
}

int change_core_size(const char *value){
    struct rlimit limit;

    long new_limit = atol(value);

    if (new_limit < 0){
        fprintf(stderr, "Invalid core size: %s\n", value);
        return -1;
    }

    limit.rlim_cur = new_limit;
    limit.rlim_max = new_limit;

    if (setrlimit(RLIMIT_CORE, &limit) == -1) {
        perror("setrlimit");
        return -1;
    }

    printf("New core size set to %ld\n", new_limit);

    return 0;
}

int print_directory(){
    char currentDirectory[1024];

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
        fprintf(stderr, "Invalid environment format: %s\n", assignment);
        fprintf(stderr, "Use: -V name=value\n");
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

void print_usage(){
    printf("Usage:\n");
    printf("  -i : Print real/effective UID and GID\n");
    printf("  -s : Become process group leader\n");
    printf("  -p : Print process IDs\n");
    printf("  -u : Show current ulimit\n");
    printf("  -U value : Change ulimit\n");
    printf("  -c : Show core size\n");
    printf("  -C value : Change core size\n");
    printf("  -d : Show current directory\n");
    printf("  -v : Print environment\n");
    printf("  -V name=value : Set environment variable\n");
}

int main(int argc, char *argv[]){
    char options[] = "ispucdvU:C:V:";
    int c;

    if (argc == 1) {
        printf("No options specified.\n");
        print_usage();
        return 0;
    }

    // Проверка на случай, если аргумент содержит несколько флагов после одного '-'
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && strlen(argv[i]) > 2 && argv[i][1] != 'U' && argv[i][1] != 'C' && argv[i][1] != 'V') {
            fprintf(stderr, "Разделяйте опции пробелом: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }
    
    char **reversed_argv = malloc((argc + 1) * sizeof(char *));
    if (!reversed_argv) {
        perror("malloc failed");
        return 1;
    }
    
    reversed_argv[0] = argv[0];
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
                    fprintf(stderr, "Failed to set ulimit.\n");
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
                    fprintf(stderr, "Failed to set core size.\n");
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
                    fprintf(stderr, "Failed to set environment variable.\n");
                    return 1;
                }
                break;
            case '?':
                fprintf(stderr, "Invalid option: -%c\n", optopt);
                print_usage();
                return 1;
        }
    }
    
    return 0;
}


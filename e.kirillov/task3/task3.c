#include <unistd.h>
#include <stdio.h>


void tryToOpen(char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Failed to open file\n");
        return;
    }

    fclose(file);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: ./task3 <path/to/file>\n");
        return -1;
    }

    printf("uid: %d, euid: %d\n", getuid(), geteuid());
    tryToOpen(argv[1]);

    setuid(getuid());

    printf("uid: %d, euid: %d\n", getuid(), geteuid());
    tryToOpen(argv[1]);

    return 0;
}
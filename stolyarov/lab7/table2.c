#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

static sigjmp_buf env;

void alarm_handler(int sig) {
    (void)sig;
    siglongjmp(env, 1);
}

int main(void) {
    int fd = open("file.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) < 0) {
        perror("fstat");
        close(fd);
        return 1;
    }

    if (sb.st_size == 0) {
        puts("Пустой файл");
        close(fd);
        return 0;
    }

    char *mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    int *mas = NULL;
    int count = 0;
    size_t lines = 0;

    for (off_t i = 0; i < sb.st_size; i++) {
        if (mapped[i] == '\n') {
            int *tmp = realloc(mas, (count + 2) * sizeof(int));
            if (!tmp) {
                perror("realloc");
                free(mas);
                munmap(mapped, sb.st_size);
                close(fd);
                return 1;
            }
            mas = tmp;

            if (count == 0) {
                mas[count] = (int)i;
                mas[count + 1] = (int)i;
            } else {
                int prev_end = mas[count - 1];
                mas[count] = (int)(i - prev_end - 1);
                mas[count + 1] = (int)i;
            }
            count += 2;
            lines++;
        }
    }

    if (sb.st_size > 0 && (lines == 0 || mas[count - 1] != (int)(sb.st_size - 1))) {
        int *tmp = realloc(mas, (count + 2) * sizeof(int));
        if (!tmp) {
            perror("realloc");
            free(mas);
            munmap(mapped, sb.st_size);
            close(fd);
            return 1;
        }
        mas = tmp;
        int prev_end = (count == 0) ? -1 : mas[count - 1];
        mas[count]     = (int)(sb.st_size - prev_end - 1);
        mas[count + 1] = (int)(sb.st_size - 1);
        count += 2;
        lines++;
    }

    printf("=========table==========\n");
    for (int i = 0; i < count; i += 2) {
        printf("# %d len = %d, end = %d\n", i/2 + 1, mas[i], mas[i+1]);
    }
    printf("========================\n");

    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    bool first_prompt_with_timer = true;

    while (1) {
        int ind;

        if (sigsetjmp(env, 1) == 0) {
            if (first_prompt_with_timer) {
                printf("Дано 5 секунд на ввод номера строки\n");
                alarm(5);
            } else {
                printf("Введите номер строки (без таймера)\n");
            }

            char buffer[100];
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                alarm(0);
                first_prompt_with_timer = false;

                if (sscanf(buffer, "%d", &ind) != 1) {
                    printf("Неверный формат числа\n");
                    continue;
                }

                if (ind == 0) {
                    printf("end\n");
                    break;
                }

                if (ind > 0 && (size_t)ind <= lines) {
                    size_t idx_len = (size_t)((ind - 1) * 2);
                    size_t idx_end = idx_len + 1;

                    int len = mas[idx_len];
                    int end_pos = mas[idx_end];

                    off_t start = (ind == 1) ? 0 : (mas[idx_end - 2] + 1);

                    if (len > 0) {
                        (void)fwrite(mapped + start, 1, (size_t)len, stdout);
                    }
                    putchar('\n');
                } else {
                    printf("неверный индекс\n");
                }
            } else {
                printf("Ошибка ввода\n");
            }
        } else {
            printf("Время вышло\n");
            first_prompt_with_timer = false;
            alarm(0);
            (void)fwrite(mapped, 1, (size_t)sb.st_size, stdout);
            putchar('\n');
            break;
        }
    }

    munmap(mapped, sb.st_size);
    close(fd);
    free(mas);
    return 0;
}

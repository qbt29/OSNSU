#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/uppercase_socket"
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

volatile sig_atomic_t keep_running = 1;
int signal_pipe[2];

/* Обработчик сигналов */
void signal_handler(int sig) {
    (void)sig;
    keep_running = 0;
    /* Будим select */
    write(signal_pipe[1], "x", 1);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    int server_fd, max_fd, activity, new_client_fd;
    int client_sockets[MAX_CLIENTS] = {0};
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    fd_set readfds;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    /* Pipe для сигналов */
    if (pipe(signal_pipe) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /* Настройка сигналов */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  // без SA_RESTART

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* Серверный сокет */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH,
            sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен. Ctrl+C для выхода\n");

    /* Главный цикл */
    while (keep_running) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        FD_SET(signal_pipe[0], &readfds);

        max_fd = server_fd > signal_pipe[0] ?
                 server_fd : signal_pipe[0];

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
                if (client_sockets[i] > max_fd)
                    max_fd = client_sockets[i];
            }
        }

        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select");
            break;
        }

        /* Сигнал */
        if (FD_ISSET(signal_pipe[0], &readfds)) {
            char tmp[8];
            read(signal_pipe[0], tmp, sizeof(tmp));
            break;
        }

        /* Новое подключение */
        if (FD_ISSET(server_fd, &readfds)) {
            client_len = sizeof(client_addr);
            new_client_fd = accept(server_fd,
                                   (struct sockaddr *)&client_addr,
                                   &client_len);
            if (new_client_fd >= 0) {
                printf("Клиент подключен: fd=%d\n", new_client_fd);
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_sockets[i] == 0) {
                        client_sockets[i] = new_client_fd;
                        break;
                    }
                }
            }
        }

        /* Клиенты */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_sockets[i];
            if (fd > 0 && FD_ISSET(fd, &readfds)) {
                bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
                if (bytes_read <= 0) {
                    close(fd);
                    client_sockets[i] = 0;
                } else {
                    buffer[bytes_read] = '\0';
                    for (ssize_t j = 0; j < bytes_read; j++)
                        buffer[j] = toupper((unsigned char)buffer[j]);
                    printf("%d:%s ", fd, buffer);
                }
            }
        }
    }

    /* Очистка */
    printf("\nЗавершение сервера...\n");

    for (int i = 0; i < MAX_CLIENTS; i++)
        if (client_sockets[i] > 0)
            close(client_sockets[i]);

    close(server_fd);
    close(signal_pipe[0]);
    close(signal_pipe[1]);
    unlink(SOCKET_PATH);

    printf("Сервер остановлен корректно\n");
    return 0;
}

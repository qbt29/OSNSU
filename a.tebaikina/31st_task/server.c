#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/select.h>
#include <time.h>

#define NUM_CLIENTS 10    // макс число клиентов

// структура с данными клиента
typedef struct {
    int fd;               // файловый дескриптор клиента
    struct timespec start_ts; // время подключения клиента
    int active;           // активность клиента
    int num;              // идентификатор клиента
} client_info_t;

client_info_t clients[NUM_CLIENTS];

// низкоуровневое чтение из клиента
int read_(int idx) {
    char c;
    int fd = clients[idx].fd;
    int bytes = read(fd, &c, 1); // читаем один байт

    if (bytes > 0) {
        putchar(toupper((unsigned char)c)); // печатаем в верхнем регистре
        fflush(stdout);
        return 0;
    }

    // если байтов нет или ошибка — закрываем клиента
    if (bytes <= 0) {
        if (bytes == -1) perror("read"); // выводим ошибку если есть
        close(fd); // закрываем сокет клиента

        // берем время завершения работы
        struct timespec end_ts;
        clock_gettime(CLOCK_MONOTONIC, &end_ts);

        // считаем длительность соединения
        double diff = (end_ts.tv_sec - clients[idx].start_ts.tv_sec) +
                      (end_ts.tv_nsec - clients[idx].start_ts.tv_nsec) / 1e9;

        // вывод текущего времени
        time_t now = time(NULL);
        struct tm *tm = localtime(&now);
        char tbuf[64];
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", tm);

        printf("\n[%d] END %s (DIFF %.6f sec)\n",
               clients[idx].num, tbuf, diff);

        clients[idx].active = 0; // помечаем клиента как неактивного
        return 1;
    }

    return 0;
}

int main () {
    printf("\n сервер \n");

    int server_fd = socket(PF_LOCAL, SOCK_STREAM, 0); // создаем unix-сокет
    if (server_fd == -1) { perror("socket"); return 1; }

    const char* path_socket = "./socket";

    // удаляем старый сокетный файл если он существует
    if (unlink(path_socket) == -1 && errno != ENOENT) {
        perror("unlink");
        close(server_fd);
        return 1;
    }

    struct sockaddr_un address;
    memset(&address, 0, sizeof(address)); // обнуляем структуру
    address.sun_family = AF_UNIX;         // тип адреса
    strncpy(address.sun_path, path_socket, sizeof(address.sun_path)-1); // путь

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("bind"); close(server_fd); return 1;
    }

    // слушаем входящие соединения
    if (listen(server_fd, NUM_CLIENTS+1) == -1) {
        perror("listen"); close(server_fd); return 1;
    }

    // инициализация клиентов
    for (int i = 0; i < NUM_CLIENTS; i++) {
        clients[i].fd = -1;
        clients[i].active = 0;
        clients[i].num = i+1; // назначаем номер клиенту
    }

    fd_set rfds; // набор дескрипторов для select

    while(1) {
        FD_ZERO(&rfds);            // очищаем набор
        FD_SET(server_fd, &rfds);  // добавляем серверный сокет
        int mx_fd = server_fd;     // максимальный дескриптор

        // добавляем активных клиентов
        for (int i = 0; i < NUM_CLIENTS; i++) {
            if (clients[i].active) {
                FD_SET(clients[i].fd, &rfds);
                if (clients[i].fd > mx_fd) mx_fd = clients[i].fd;
            }
        }

        // ждем событий
        int action = select(mx_fd+1, &rfds, NULL, NULL, NULL);
        if (action == -1) { perror("select"); break; }

        // подключение нового клиента
        if (FD_ISSET(server_fd, &rfds)) {
            struct sockaddr_un client_addr;
            socklen_t client_len = sizeof(client_addr);

            int fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            if (fd == -1) { perror("accept"); continue; }

            // ищем свободное место
            int fl = 0;
            for (int i = 0; i < NUM_CLIENTS; i++) {
                if (!clients[i].active) {
                    clients[i].fd = fd;
                    clients[i].active = 1;
                    clock_gettime(CLOCK_MONOTONIC, &clients[i].start_ts); // фиксируем время старта

                    // выводим старт клиента
                    time_t now = time(NULL);
                    struct tm *tm = localtime(&now);
                    char tbuf[64];
                    strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", tm);
                    printf("\n[%d] START %s\n", clients[i].num, tbuf);

                    fl = 1;
                    break;
                }
            }

            // если мест нет — закрываем соединение
            if (!fl) close(fd);
        }

        // читаем данные от активных клиентов
        for (int i = 0; i < NUM_CLIENTS; i++) {
            if (clients[i].active && FD_ISSET(clients[i].fd, &rfds)) {
                read_(i);
            }
        }
    }

        for (int i = 0; i < NUM_CLIENTS; i++)
        if (clients[i].active) close(clients[i].fd);

    close(server_fd);
    return 0;
}

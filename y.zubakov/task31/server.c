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

const char* path_socket = "./socket";
#define MAX_CLIENTS 5

typedef struct {
    int fd;
    struct timespec start_ts;
    int active;
    int id;
} client_info_t;

client_info_t clients[MAX_CLIENTS];

int read_client(int id) {
    char c;
    int fd = clients[id].fd;
    int bytes = read(fd, &c, 1);

    if (bytes > 0) {
        putchar(toupper((unsigned char)c));
        fflush(stdout);
        return 0;
    }

    if (bytes <= 0) {
        if (bytes == -1)
            perror("read"); 
        close(fd);


        struct timespec end_ts;
        clock_gettime(CLOCK_MONOTONIC, &end_ts);

        double diff = (end_ts.tv_sec - clients[id].start_ts.tv_sec) + (end_ts.tv_nsec - clients[id].start_ts.tv_nsec) / 1e9;

        time_t now = time(NULL);
        struct tm *tm = localtime(&now);
        char tbuf[64];
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", tm);

        printf("\n[%d] END %s (DIFF %.6f sec)\n", clients[id].id, tbuf, diff);

        clients[id].active = 0;
        return 1;
    }

    return 0;
}

int main () {
    printf("------Запуск сервера------\n");

    int server_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (server_fd == -1) { 
        perror("socket"); 
        return 1; 
    }

    if (unlink(path_socket) == -1 && errno != ENOENT) { //удаление старого файла скоета
        perror("unlink");
        close(server_fd);
        return 1;
    }

    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, path_socket, sizeof(address.sun_path)-1);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("bind"); 
        close(server_fd); 
        return 1;
    }
    if (listen(server_fd, MAX_CLIENTS+1) == -1) {
        perror("listen"); 
        close(server_fd); 
        return 1;
    }
    
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].fd = -1;
        clients[i].active = 0;
        clients[i].id = i;
    }
    
    fd_set rfds;
    
    printf("------Сервер запущен------\n");
    while(1) {
        FD_ZERO(&rfds);            
        FD_SET(server_fd, &rfds);
        int max_fd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].active) {
                FD_SET(clients[i].fd, &rfds);
                if (clients[i].fd > max_fd) 
                    max_fd = clients[i].fd;
            }
        }

        int action = select(max_fd+1, &rfds, NULL, NULL, NULL);
        if (action == -1) { 
            perror("select"); 
            break; 
        }

        if (FD_ISSET(server_fd, &rfds)) {
            struct sockaddr_un client_addr;
            socklen_t client_len = sizeof(client_addr);

            int fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            if (fd == -1) { 
                perror("accept"); 
                continue; 
            }

            int flag = 0;
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (!clients[i].active) {
                    clients[i].fd = fd;
                    clients[i].active = 1;
                    clock_gettime(CLOCK_MONOTONIC, &clients[i].start_ts); 

                    time_t now = time(NULL);
                    struct tm *tm = localtime(&now);
                    char tbuf[64];
                    strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", tm);
                    printf("\n[%d] START %s\n", clients[i].id, tbuf);

                    flag = 1;
                    break;
                }
            }

            if (!flag) 
                close(fd);
        }

        for (int i = 0; i < MAX_CLIENTS; ++i)
            if (clients[i].active && FD_ISSET(clients[i].fd, &rfds))
                read_client(i);
    }

    //закрытие всех оставшихся сокетов
    for (int i = 0; i < MAX_CLIENTS; ++i) 
        if (clients[i].active) 
            close(clients[i].fd);
    printf("\n------Обработка завершена------\n");
    close(server_fd);
    return 0;
}

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

const char* path_socket = "./socket";

void read_buff (int fd) {
    char c;
    int bytes;
    while ((bytes = read(fd, &c, 1)) > 0) {
        putchar(toupper(c));
    }
    printf("\n");
};


int main () {

    printf("------Запуск сервера------\n");

    int fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return 1;
    }
    
    if (unlink(path_socket) == -1 && errno != 2) {
        perror("unlink");
        close(fd);
        return 1;
    }

    struct sockaddr_un address; 
    address.sun_family = PF_LOCAL;
    strncpy(address.sun_path, path_socket, sizeof(address.sun_path) - 1);

    if (bind(fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("bind");
        close(fd);
        return 1;
    }
    printf("------Сервер запущен------\n");
    if (listen(fd, 1) == -1) { //слушаем (максимум 1 пользователь)
        perror("listen");
        close(fd);
        return 1;
    }

    socklen_t len_address = sizeof(address);
    int client_fd = accept(fd, (struct sockaddr*)&address, &len_address);
    if (client_fd == -1) {
        perror("accept");
        close(fd);
        return 1;
    }
    printf("Получено сообщение: ");
    read_buff(client_fd);

    close(client_fd);
    close(fd);
    return 0;
}
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


const char* path_socket = "./socket";


int main () {
    int fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return 1;
    }
    
    struct sockaddr_un address;
    address.sun_family = PF_LOCAL;
    strncpy(address.sun_path, path_socket, sizeof(address.sun_path) - 1);
    printf("-----Подключение...-----\n");
    if (connect(fd, (struct sockaddr*)&address, sizeof(address)) == -1) { //инициализация подключения
        perror("connect");
        close(fd);
        return 1;
    }
    printf("-----Подключено-----\n");
    char msg[1024] = "Some client text\n";
    // getline(msg, 1024, stdin);

    write(fd, msg, strlen(msg));
    printf("-----Сообщение доставлено-----\n");
    close(fd);
    return 0;
}
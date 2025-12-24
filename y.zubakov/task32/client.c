#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SOCKET_PATH "./socket"

void msend(int fd, int num) {
    const char* msgs[] = {
        "fir_msg", "sec_msg", "thi_msg", "fou_msg", "fif_msg"
    };

    char buf[64];
    for (int i = 0; i < 100; ++i) {
        const char* msg = msgs[rand() % 5];
        snprintf(buf, sizeof(buf), "[%d] %s\n", num, msg);
        write(fd, buf, strlen(buf));
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    int fd;
    struct sockaddr_un address;
    printf("-----Подключение...-----\n");
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) { perror("socket"); return 1; }

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    if (connect(fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("connect"); close(fd); return 1;
    }
    printf("-----Подключено-----\n");
    int num = argc > 1 ? atoi(argv[1]) : 1;
    msend(fd, num);
    printf("-----Сообщение доставлено-----\n");
    close(fd);
    return 0;
}

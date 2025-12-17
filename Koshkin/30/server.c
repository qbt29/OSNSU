#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/uppercase_socket"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char ch;
    ssize_t bytes_read;

    // Создаем сокет
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Удаляем старый сокет, если он существует
    unlink(SOCKET_PATH);

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Привязываем сокет к адресу
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Начинаем слушать входящие соединения
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен. Ожидание подключения клиента...\n");

    // Принимаем подключение клиента (ждем только одно подключение)
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Клиент подключен. Принимаю символы:\n");

    // Принимаем и обрабатываем символы по одному
    while (1) {
        bytes_read = read(client_fd, &ch, 1);
        
        if (bytes_read <= 0) {
            // Клиент отключился
            if (bytes_read < 0) {
                perror("read");
            }
            break;
        }
        
        // Преобразуем символ в верхний регистр и выводим
        char upper_ch = toupper((unsigned char)ch);
        putchar(upper_ch);
        fflush(stdout);
        
        // Если получен символ новой строки, выводим возврат каретки
        if (ch == '\n') {
            printf("\r");
        }
    }

    printf("\nКлиент отключился. Завершение работы сервера.\n");
    
    // Закрываем соединения
    close(client_fd);
    close(server_fd);
    
    // Удаляем файл сокета
    unlink(SOCKET_PATH);

    return 0;
}
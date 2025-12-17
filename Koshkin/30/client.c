#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <termios.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/uppercase_socket"

static struct termios old_termios;

void disable_canonical_mode() {
    struct termios new_termios;
    
    tcgetattr(STDIN_FILENO, &old_termios);
    
    new_termios = old_termios;
    
    // Отключаем канонический режим, эхо и обработку специальных символов
    new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);
    
    // Минимальное количество символов для чтения = 1, без таймаута
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void restore_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

void signal_handler(int sig) {
    restore_terminal_mode();
    printf("\nПолучен сигнал %d. Завершение работы.\n", sig);
    exit(0);
}

int main() {
    int sock_fd;
    struct sockaddr_un server_addr;
    char ch;
    
    // Устанавливаем обработчики сигналов
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Сначала подключаемся к серверу
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        printf("Убедитесь, что сервер запущен.\n");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Подключено к серверу.\n");
    printf("Вводите текст (символы отправляются сразу, Ctrl+D для завершения):\n");
    
    // Включаем raw-режим терминала
    disable_canonical_mode();
    
    // Читаем и отправляем символы по одному
    while (1) {
        ch = getchar();
        
        // Проверяем на EOF (Ctrl+D) или Ctrl+C
        if (ch == EOF || ch == 4) { // 4 = ASCII код Ctrl+D
            printf("\nПолучен Ctrl+D. Завершение ввода.\n");
            break;
        }
        
        // Отображаем введенный символ
        putchar(ch);
        fflush(stdout);
        
        // Отправляем символ серверу
        if (write(sock_fd, &ch, 1) < 0) {
            perror("write");
            break;
        }
        
        // Если нажали Enter, отправляем также символ новой строки
        if (ch == '\n') {
            printf("\r"); // Возврат каретки для красивого вывода
        }
    }
    
    // Восстанавливаем режим терминала
    restore_terminal_mode();
    
    printf("Разрыв соединения.\n");
    close(sock_fd);

    return 0;
}
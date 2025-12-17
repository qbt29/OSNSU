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
    
    // Отключаем канонический режим и эхо
    new_termios.c_lflag &= ~(ICANON | ECHO);
    
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

int main(int argc, char *argv[]) {
    int sock_fd;
    struct sockaddr_un server_addr;
    char buffer[1024];
    int client_id = getpid(); // Используем PID как идентификатор клиента
    
    // Устанавливаем обработчики сигналов
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Создаем сокет
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Подключаемся к серверу
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        printf("Убедитесь, что сервер запущен.\n");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Клиент %d подключен к серверу.\n", client_id);
    printf("Введите текст (Ctrl+D для завершения):\n");
    
    if (argc > 1) {
        // Если текст передан как аргумент командной строки
        snprintf(buffer, sizeof(buffer), "%s\n", argv[1]);
        write(sock_fd, buffer, strlen(buffer));
        printf("Отправлено: %s", buffer);
    } else {
        // Интерактивный режим
        disable_canonical_mode();
        
        while (1) {
            char ch = getchar();
            
            // Проверяем на EOF (Ctrl+D)
            if (ch == EOF || ch == 4) {
                printf("\nЗавершение ввода.\n");
                break;
            }
            
            // Отправляем символ серверу
            if (write(sock_fd, &ch, 1) < 0) {
                perror("write");
                break;
            }
            
            // Если Enter, отправляем также символ новой строки
            if (ch == '\n') {
                putchar(ch);
                fflush(stdout);
            } else {
                putchar(ch);
                fflush(stdout);
            }
        }
        
        restore_terminal_mode();
    }
    
    printf("Разрыв соединения.\n");
    close(sock_fd);
    
    return 0;
}
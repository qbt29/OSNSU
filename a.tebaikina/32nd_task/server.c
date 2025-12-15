#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <aio.h>

#define NUM_CLIENTS 10       // макс число клиентов
#define SOCKET_PATH "./socket"  // путь к unix-сокету
#define BLOCK_SIZE 4         // читаем по 4 байта

// структура клиента
typedef struct {
    int fd;                  // файловый дескриптор клиента
    int active;              // активность клиента
    int num;                 // номер клиента
    struct aiocb aio;        // aio контрольный блок
    char buf[BLOCK_SIZE+1];  // буфер чтения + завершающий нуль
} client_t;

client_t clients[NUM_CLIENTS]; // массив клиентов
int server_fd;                 // сокет сервера

// печать события с точным временем
void print_time_event(int num, const char *event) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); // берем текущее время

    struct tm tm_info;
    localtime_r(&ts.tv_sec, &tm_info); // преобразуем в локальное время

    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_info); // форматируем вывод

    printf("[%d] %s %s.%03ld\n", num, event, buf, ts.tv_nsec / 1000000);
    fflush(stdout);
}

// запуск асинхронного чтения aio_read
void start_aio(client_t *c) {
    memset(&c->aio, 0, sizeof(c->aio)); // обнуляем структуру aio
    c->aio.aio_fildes = c->fd;          // дескриптор клиента
    c->aio.aio_buf = c->buf;            // куда читать
    c->aio.aio_nbytes = BLOCK_SIZE;     // сколько байт
    c->aio.aio_offset = 0;              // смещение не используется

    aio_read(&c->aio);                  // запускаем асинхронное чтение
}

int main() {

    printf("\n сервер запущен \n");

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // создаем unix-сокет
    if (server_fd < 0) { perror("socket"); return 1; }

    unlink(SOCKET_PATH); // удаляем старый сокет если был

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr)); // обнуляем структуру адреса
    addr.sun_family = AF_UNIX;      // тип сокета
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1); // путь

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }

    if (listen(server_fd, NUM_CLIENTS) < 0) { perror("listen"); return 1; }

    for (int i=0; i<NUM_CLIENTS; i++) clients[i].active = 0; // помечаем всех как неактивных

    fcntl(server_fd, F_SETFL, O_NONBLOCK); // делаем сервер неблокирующим

    while (1) {

        // попытка принять клиента
        int fd = accept(server_fd, NULL, NULL);
        if (fd > 0) {
            int idx = -1;
            // ищем свободное место для клиента
            for (int i=0; i<NUM_CLIENTS; i++)
                if (!clients[i].active) { idx = i; break; }

            if (idx >= 0) {
                clients[idx].fd = fd;
                clients[idx].active = 1;
                clients[idx].num = idx + 1;

                print_time_event(clients[idx].num, "START"); // логируем старт клиента

                fcntl(fd, F_SETFL, O_NONBLOCK); // делаем дескриптор неблокирующим

                start_aio(&clients[idx]); // запускаем первое aio чтение
            } else {
                close(fd); // нет мест
            }
        }

        // проверяем завершение асинхронного чтения
        for (int i=0; i<NUM_CLIENTS; i++) {
            if (!clients[i].active) continue;

            int err = aio_error(&clients[i].aio); // проверяем статус операции

            if (err == 0) { // завершено успешно
                int n = aio_return(&clients[i].aio); // получаем число прочитанных байт

                if (n > 0) {
                    clients[i].buf[n] = 0; // завершаем строку

                    // переводим в верхний регистр
                    for (int j=0; j<n; j++)
                        clients[i].buf[j] = toupper((unsigned char)clients[i].buf[j]);

                    printf("%s", clients[i].buf); // вывод сервера
                    fflush(stdout);

                    start_aio(&clients[i]); // запускаем следующее чтение
                }
                else if (n == 0) { // клиент закрыл соединение
                    close(clients[i].fd);
                    print_time_event(clients[i].num, "END"); // логируем завершение
                    clients[i].active = 0;
                }
            }
            else if (err != EINPROGRESS) { // ошибка не в процессе
                perror("aio_error");
                close(clients[i].fd);
                clients[i].active = 0;
            }
        }

        usleep(1000); // немного спим чтобы не жрать cpu
    }

    close(server_fd); // закрываем сервер
    return 0;
}

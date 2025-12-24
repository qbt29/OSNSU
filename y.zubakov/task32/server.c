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

const char* path_socket = "./socket";
#define MAX_CLIENTS 5
#define BLOCK_SIZE 6

typedef struct {
    int fd;
    int active;
    int id;
    struct aiocb aio;
    char buff[BLOCK_SIZE+1]; //+1 - \0
} client_info_t;

client_info_t clients[MAX_CLIENTS];

void print_event_time(int id, const char *event) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    struct tm tm_info;
    localtime_r(&ts.tv_sec, &tm_info);

    char buff[64];
    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm_info);

    printf("[%d] %s %s.%03ld\n", id, event, buff, ts.tv_nsec / 1000000);
    fflush(stdout);
}

void start_aio(client_info_t *c) {
    memset(&c->aio, 0, sizeof(c->aio));
    c->aio.aio_fildes = c->fd;
    c->aio.aio_buf = c->buff;
    c->aio.aio_nbytes = BLOCK_SIZE;
    c->aio.aio_offset = 0;

    aio_read(&c->aio);
}

void process_client(int id) {
    int err = aio_error(&clients[id].aio);
    if(err == 0) {
        int n = aio_return(&clients[id].aio);
        if(n > 0) {
            clients[id].buff[n] = 0;
            for(int j = 0; j < n; ++j) 
                clients[id].buff[j] = toupper((unsigned char)clients[id].buff[j]);
            printf("%s", clients[id].buff);
            fflush(stdout);
            start_aio(&clients[id]);
        } 
        else if(n == 0) {
            close(clients[id].fd);
            print_event_time(clients[id].id, "END");
            clients[id].active=0;
        }
    } 
    else if(err != EINPROGRESS) {
        perror("aio_error");
        close(clients[id].fd);
        clients[id].active=0;
    }
}

int main () {
    printf("------Запуск сервера------\n");

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
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

    printf("------Сервер запущен------\n");
    
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].fd = -1;
        clients[i].active = 0;
        clients[i].id = i;
    }

    fd_set rfds;

    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    while(1) {
        int fd = accept(server_fd, NULL, NULL);
        if(fd > 0){
            int id = -1;
            for(int i=0;i<MAX_CLIENTS;++i) 
                if(!clients[i].active){ 
                    id=i; 
                    break; 
                }
            if(id >= 0){
                clients[id].fd = fd;
                clients[id].active = 1;
                clients[id].id = id;
                print_event_time(clients[id].id, "START");
                fcntl(fd, F_SETFL, O_NONBLOCK);
                start_aio(&clients[id]);
            } 
            else 
                close(fd);
        }

        for(int i = 0; i < MAX_CLIENTS; ++i){
            if(!clients[i].active) 
                continue;
            process_client(i);
        }
        usleep(1000);
    }

    //закрытие всех оставшихся сокетов
    for (int i = 0; i < MAX_CLIENTS; ++i) 
        if (clients[i].active) 
            close(clients[i].fd);
    printf("\n------Обработка завершена------\n");
    close(server_fd);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    printf("Клиент подключен: %s\n", client_ip);

    srand(time(NULL));
    int number = rand() % 100 + 1;
    printf("Загадано число: %d\n", number);

    char buffer[BUFFER_SIZE];
    int attempts = 0;

    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_sock, buffer, BUFFER_SIZE-1, 0);

        if(bytes <= 0) {
            printf("Клиент отключился\n");
            close(client_sock);
            return;
        }

        int guess = atoi(buffer);
        attempts++;

        if(guess < number) {
            strcpy(buffer, "higher");
        } else if(guess > number) {
            strcpy(buffer, "lower");
        } else {
            strcpy(buffer, "correct");
            printf("Число угадано за %d попыток\n", attempts);
            send(client_sock, buffer, strlen(buffer), 0);
            close(client_sock);
            return;
        }

        send(client_sock, buffer, strlen(buffer), 0);
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Использование: %s <порт>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        perror("Ошибка сокета");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка bind");
        close(server_fd);
        return 1;
    }

    if(listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Ошибка listen");
        close(server_fd);
        return 1;
    }

    printf("Сервер запущен на порту %d\n", port);

    while(1) {
        client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if(client_sock < 0) {
            perror("Ошибка accept");
            continue;
        }

        if(fork() == 0) {
            close(server_fd);
            handle_client(client_sock, client_addr);
            exit(0);
        }
        close(client_sock);
    }

    close(server_fd);
    return 0;
}

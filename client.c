#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Использование: %s <IP> <порт>\n", argv[0]);
        return 1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("Ошибка сокета");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Неверный адрес");
        close(sock);
        return 1;
    }

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка подключения");
        close(sock);
        return 1;
    }

    printf("Подключено к серверу. Угадайте число (1-100):\n");

    char buffer[BUFFER_SIZE];
    while(1) {
        printf("Ваш вариант: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if(send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Ошибка отправки");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        if(recv(sock, buffer, BUFFER_SIZE-1, 0) <= 0) {
            perror("Ошибка получения");
            break;
        }

        if(strcmp(buffer, "correct") == 0) {
            printf("Поздравляем! Вы угадали!\n");
            break;
        }

        printf("Сервер: нужно %s\n", buffer);
    }

    close(sock);
    return 0;
}

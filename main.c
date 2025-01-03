#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "file_utils/file_utils.h"
#include "prefix_tree/prefix_tree.h"


#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define NUM_PCS 1


int main(const int argc, char *argv[]) {
    char* fileName = "/home/ivan/CLionProjects/untitled/file.txt";
    // long* file_parts = split_file(fileName, NUM_PCS);

    int client_socket;
    struct sockaddr_in server_addr;

    // Создаем сокет
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Настроим серверный адрес
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Подключаемся к серверу
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Receiving file...\n");

    // Отправляем файл клиенту
    send_file(client_socket, fileName);

    close(client_socket);

    return 0;
}
// /home/ivan/CLionProjects/untitled/file.txt
#include "file_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void send_file(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("File opening failed");
        close(client_socket);
        return;
    }

    char buffer[1024];
    size_t bytes_read;

    // Отправляем файл клиенту
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, bytes_read, 0) == -1) {
            perror("Send failed");
            break;
        }
    }

    fclose(file);
    close(client_socket);
}

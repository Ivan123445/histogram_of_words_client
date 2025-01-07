#include "file_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// void send_file(int client_socket, const char *filename) {
//     FILE *file = fopen(filename, "r");
//     if (file == NULL) {
//         perror("File opening failed");
//         close(client_socket);
//         return;
//     }
//
//     char buffer[1024];
//     size_t bytes_read;
//
//     // Отправляем файл клиенту
//     while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
//         if (send(client_socket, buffer, bytes_read, 0) == -1) {
//             perror("Send failed");
//             break;
//         }
//     }
//
//     fclose(file);
//     close(client_socket);
// }

void send_file(int client_socket, const char *filename, long start, long end) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("File opening failed");
        close(client_socket);
        return;
    }

    if (start < 0 || end < start) {
        fprintf(stderr, "Invalid start or end values\n");
        fclose(file);
        close(client_socket);
        return;
    }

    if (fseek(file, start, SEEK_SET) != 0) {
        perror("Seek failed");
        fclose(file);
        close(client_socket);
        return;
    }

    char buffer[1024];
    size_t bytes_to_read;
    long bytes_remaining = end - start + 1;
    send(client_socket, (char *)&bytes_remaining, sizeof(bytes_remaining), 0);

    while (bytes_remaining > 0) {
        bytes_to_read = (bytes_remaining < sizeof(buffer)) ? bytes_remaining : sizeof(buffer);
        size_t bytes_read = fread(buffer, 1, bytes_to_read, file);
        if (bytes_read <= 0) {
            if (feof(file)) {
                break;
            }
            perror("File read failed");
            break;
        }

        if (send(client_socket, buffer, bytes_read, 0) == -1) {
            perror("Send failed");
            break;
        }

        bytes_remaining -= bytes_read;
        // printf("Bytes read: %ld\n", bytes_read);
        // printf("Bytes remaining: %ld\n", bytes_remaining);
    }
    send(client_socket, "\n", 1, 0);
    // printf("File sent\n");
    fclose(file);
}


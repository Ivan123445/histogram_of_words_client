#include "file_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>


void generate_filename(char *filename) {
    srand(time(0));
    int random_number = rand() % 10000;

    snprintf(filename, MAX_FILENAME_LEN, "%s_%d", FILENAME_PREFIX, random_number);
    printf("%s\n", filename);
}

void write_file_from_console(char *filename) {
    generate_filename(filename);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        exit(EXIT_FAILURE);
    }

    printf("Введите текст. Для завершения введите строку \"STOP\":\n");

    char buffer[1024];
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("Ошибка ввода");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        if (strncmp(buffer, "STOP\n", 5) == 0) {
            break;
        }

        if (fputs(buffer, file) == EOF) {
            perror("Ошибка записи в файл");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    if (fclose(file) == EOF) {
        perror("Ошибка закрытия файла");
        exit(EXIT_FAILURE);
    }

    printf("Текст успешно записан!\n");
}

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


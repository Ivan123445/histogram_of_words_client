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
        perror("File opening errorS");
        exit(EXIT_FAILURE);
    }

    printf("Enter the text. To complete, enter the line \"STOP\":\n");

    char buffer[1024];
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("Input error");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        if (strncmp(buffer, "STOP\n", 5) == 0) {
            break;
        }

        if (fputs(buffer, file) == EOF) {
            perror("Error writing to a file");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    if (fclose(file) == EOF) {
        perror("File closing error");
        exit(EXIT_FAILURE);
    }

    printf("The text has been recorded successfully!\n");
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
    long bytes_remaining = end - start;
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

void write_file(char *input_filename, char *output_filename, long start, long end) {
    if (start < 0 || end < 0 || start > end) {
        fprintf(stderr, "Error: invalid range (start: %ld, end: %ld)\n", start, end);
        return;
    }

    FILE *input_file = fopen(input_filename, "rb");
    if (!input_file) {
        perror("Error opening input file");
        return;
    }

    FILE *output_file = fopen(output_filename, "wb");
    if (!output_file) {
        perror("Error opening output file");
        fclose(input_file);
        return;
    }

    if (fseek(input_file, start, SEEK_SET) != 0) {
        perror("Error seeking in input file");
        fclose(input_file);
        fclose(output_file);
        return;
    }

    size_t bytes_to_copy = end - start;
    char buffer[1024];
    size_t bytes_read, bytes_written;

    while (bytes_to_copy > 0) {
        size_t chunk_size = (bytes_to_copy < sizeof(buffer)) ? bytes_to_copy : sizeof(buffer);
        bytes_read = fread(buffer, 1, chunk_size, input_file);

        if (bytes_read == 0) {
            if (ferror(input_file)) {
                perror("Error reading input file");
            }
            break;
        }

        bytes_written = fwrite(buffer, 1, bytes_read, output_file);
        if (bytes_written != bytes_read) {
            perror("Error writing to output file");
            break;
        }

        bytes_to_copy -= bytes_read;
    }

    fclose(input_file);
    fclose(output_file);
}

void print_file_part(char *input_filename, long start, long end) {
    if (start < 0 || end < 0 || start > end) {
        fprintf(stderr, "Error: invalid range (start: %ld, end: %ld)\n", start, end);
        return;
    }

    FILE *input_file = fopen(input_filename, "rb");
    if (!input_file) {
        perror("Error opening input file");
        return;
    }

    if (fseek(input_file, start, SEEK_SET) != 0) {
        perror("Error seeking in input file");
        fclose(input_file);
        return;
    }

    size_t bytes_to_copy = end - start;
    char buffer[1024];
    size_t bytes_read;

    while (bytes_to_copy > 0) {
        size_t chunk_size = (bytes_to_copy < sizeof(buffer)) ? bytes_to_copy : sizeof(buffer);
        bytes_read = fread(buffer, 1, chunk_size, input_file);

        if (bytes_read == 0) {
            if (ferror(input_file)) {
                perror("Error reading input file");
            }
            break;
        }

        printf("%s", buffer);

        bytes_to_copy -= bytes_read;
    }
    fclose(input_file);
}

void print_file_parts(char *filename, long *file_parts, int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        printf("Part %d: \n", i);
        print_file_part(filename, file_parts[i], file_parts[i + 1]);
    }
}
